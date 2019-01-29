/// \file
/// \Example macro to use TMPIFile.cxx
/// \This macro shows the usage of TMPIFile to simulate event reconstruction
///  and merging them in parallel
/// \To run this macro, once compiled, execute "mpirun -np <number of processors> ./bin/test_tmpi"
/// \macro_code
/// \Author Amit Bashyal

#include "TMPIFile.h"
#include "TFile.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TTree.h"
#include "TSystem.h"
#include "TMemFile.h"
#include "TH1D.h"
#include "TError.h"
#include "JetEvent.h"

#include "mpi.h"
#include "cxxopts.hpp"

#include <chrono>
#include <random>
#include <thread>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>


void test_tmpi(int argc,char* argv[]){


   Int_t N_collectors = 1; //specify how many collectors to receive the message
   Int_t sync_rate = 10; //events per send request by the worker
   Int_t events_per_rank = 50; // events each rank will produce
   Int_t sleep_mean = 5; // typical KNL node produces 0.1 evt / sec / node
   Int_t sleep_sigma = 2;
   Int_t jetm=25;
   Int_t trackm=60;
   Int_t hitam=200;
   Int_t hitbm=100;

   // using arg parser from here: https://github.com/jarro2783/cxxopts
   cxxopts::Options optparse("test_tmpi","runs a test of the TMPIFile class");
   optparse.add_options()
      ("c,ncollectors","number of collecting ranks to run",cxxopts::value<Int_t>(N_collectors))
      ("r,syncrate","number of events to generate per worker rank before sending to collector rank",cxxopts::value<Int_t>(sync_rate))
      ("n,evt_per_rank","total events to generate per worker rank",cxxopts::value<Int_t>(events_per_rank))
      ("s,sleep_mean","seconds to sleep, gaussian mean",cxxopts::value<Int_t>(sleep_mean))
      ("t,sleep_sigma","seconds to sleep, gaussian sigma",cxxopts::value<Int_t>(sleep_sigma))
      ("a,jetm","number of jets per event",cxxopts::value<Int_t>(jetm))
      ("b,trackm","number of tracks per jet",cxxopts::value<Int_t>(trackm))
      ("d,hitam","number of hitsA per jet",cxxopts::value<Int_t>(hitam))
      ("e,hitbm","number of hitsB per jet",cxxopts::value<Int_t>(hitbm))
      ;

   auto opts = optparse.parse(argc,argv);


   std::string mpifname("/tmp/merged_output_");
   mpifname += std::to_string(getpid());
   mpifname += ".root";

   TMPIFile *newfile = new TMPIFile(mpifname.c_str(),"RECREATE",N_collectors);
   gRandom->SetSeed(gRandom->GetSeed() + newfile->GetMPIGlobalRank());

   if(newfile->GetMPIGlobalRank() == 0){
      std::cout << " running with parallel ranks:   " << newfile->GetMPIGlobalSize() << "\n";
      std::cout << " running with collecting ranks: " << N_collectors << "\n";
      std::cout << " running with working ranks:    " << (newfile->GetMPIGlobalSize() - N_collectors) << "\n";
      std::cout << " running with sync rate:        " << sync_rate << "\n";
      std::cout << " running with events per rank:  " << events_per_rank << "\n";
      std::cout << " running with sleep mean:       " << sleep_mean << "\n";
      std::cout << " running with sleep sigma:      " << sleep_sigma << "\n";
      std::cout << " running with seed:             " << gRandom->GetSeed() << "\n";
   }

   std::cout << "[" << newfile->GetMPIGlobalRank() << "] root output filename: " << mpifname << std::endl;

   //now we need to divide the collector and worker load from here..
   if (newfile->IsCollector())
      newfile->RunCollector(); //Start the Collector Function
   else { //Workers' part
      TTree *tree = new TTree("tree","Event example with Jets");
      tree->SetAutoFlush(sync_rate);
      JetEvent *event = new JetEvent;
      tree->Branch("event","JetEvent",&event,8000,2);

      auto sync_start = std::chrono::high_resolution_clock::now();

      //total number of entries
      for(int i=0;i<events_per_rank;i++){
         auto start = std::chrono::high_resolution_clock::now();
         // std::cout<<"Event "<<i<<" local rank "<<newfile->GetMPILocalRank()<< std::endl;
         event->Build(jetm,trackm,hitam,hitbm);
         auto evt_built = std::chrono::high_resolution_clock::now();
         double build_time = std::chrono::duration_cast<std::chrono::duration<double>> (evt_built - start).count();
         std::cout << "[" << newfile->GetMPILocalRank() << "] evt = "<< i <<"; build_time = " << build_time << std::endl;
         auto adjusted_sleep = (int)(sleep_mean - build_time);
         auto sleep = abs(gRandom->Gaus(adjusted_sleep,sleep_sigma));
         //sleep after every events to simulate the reconstruction time... 
         std::this_thread::sleep_for(std::chrono::seconds(int(sleep)));
         // Fill Tree
         tree->Fill();

         //at the end of the event loop...put the sync function
         //************START OF SYNCING IMPLEMENTATION FROM USERS' SIDE**********************
         if((i+1)%sync_rate==0){
            newfile->Sync(); //this one as a worker...

            auto end = std::chrono::high_resolution_clock::now();
            double sync_time = std::chrono::duration_cast<std::chrono::duration<double>> (end - sync_start).count();
            std::cout << "[" << newfile->GetMPILocalRank() << "] event collection time: " << sync_time << std::endl;
            sync_start = std::chrono::high_resolution_clock::now();
         }
      }
      //do the syncing one more time
      if(events_per_rank%sync_rate!=0) newfile->Sync(); 

      //************END OF SYNCING IMPLEMENTATION FROM USERS' SIDE***********************
   }
   newfile->MPIClose();
}

#ifndef __CINT__
int main(int argc,char* argv[]){
    auto start = std::chrono::high_resolution_clock::now();
    
    int rank, size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    test_tmpi(argc,argv);
    MPI_Finalize();
    
    auto end = std::chrono::high_resolution_clock::now();
    double time = std::chrono::duration_cast<std::chrono::duration<double>> (end - start).count();
    std::string msg = "Total elapsed time: ";
    msg += std::to_string(time);
    msg += "\tTotal rank: ";
    msg += std::to_string(size);
    if (rank == 0)
        Info("TMPI test", msg.c_str());
    
    return 0;
}
#endif
