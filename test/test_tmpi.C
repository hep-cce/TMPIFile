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

#include <chrono>
#include <random>
#include <thread>
#include <string>
#include <iostream>

void test_tmpi(){

    Int_t N_collectors = 1; //specify how many collectors to receive the message
    Int_t sync_rate = 100; //events per send request by the worker
    Int_t events_per_rank = 1000; // events each rank will produce
    Int_t sleep_mean = 10; // typical KNL node produces 0.1 evt / sec / node
    Int_t sleep_sigma = 2; 

    char mpifname[100];
    sprintf(mpifname,"Simple_MPIFile.root");

    TMPIFile *newfile = new TMPIFile("Simple_MPIFile.root","RECREATE",N_collectors);
    Int_t seed = newfile->GetMPILocalSize()+newfile->GetMPIColor()+newfile->GetMPILocalRank();
    
    //now we need to divide the collector and worker load from here..
    if (newfile->IsCollector()) newfile->RunCollector(); //Start the Collector Function
    else { //Workers' part
        TTree *tree = new TTree("tree","Event example with Jets");
        tree->SetAutoFlush(sync_rate);
        JetEvent *event = new JetEvent;
        tree->Branch("event","JetEvent",&event,8000,2);

        Int_t sleep=0;
        //total number of entries
        for(int i=0;i<events_per_rank;i++){
            std::cout<<"Event "<<i<<" local rank "<<newfile->GetMPILocalRank()<< std::endl;
            event->Build();
            sleep = abs(gRandom->Gaus(sleep_mean,sleep_sigma));
            //sleep after every events to simulate the reconstruction time... 
            std::this_thread::sleep_for(std::chrono::seconds(sleep));
            tree->Fill();
            //at the end of the event loop...put the sync function
            //************START OF SYNCING IMPLEMENTATION FROM USERS' SIDE**********************
            if((i+1)%sync_rate==0){
                newfile->Sync(); //this one as a worker...
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
    test_tmpi();
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
