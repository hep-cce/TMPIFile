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
#include <chrono>
#include <thread>
#include "mpi.h"
#include <iostream>

void test_tmpi(){
  char mpifname[100];
  Int_t N_collectors=2; //specify how many collectors to receive the message
 Int_t sync_rate = 4; //events per send request by the worker
  sprintf(mpifname,"Simple_MPIFile.root");
  TMPIFile *newfile = new TMPIFile("Simple_MPIFile.root","RECREATE",N_collectors);
  Int_t seed = newfile->GetMPILocalSize()+newfile->GetMPIColor()+newfile->GetMPILocalRank();

 //now we need to divide the collector and worker load from here..
 if(newfile->IsCollector())newfile->RunCollector(); //Start the Collector Function
 else{ //Workers' part
TTree *tree = new TTree("tree","tree");
 tree->SetAutoFlush(400000000);
 Float_t px,py;
 Int_t reco_time;
 tree->Branch("px",&px);
 tree->Branch("py",&py);
 tree->Branch("reco_time",&reco_time);
 gRandom->SetSeed(seed);
  Int_t   sleep=0;
 //total number of entries
 Int_t tot_entries = 12;
   for(int i=0;i<tot_entries;i++){
     //    std::cout<<"Event "<<i<<" local rank "<<newfile->GetMPILocalRank()<<std::endl;
     gRandom->Rannor(px,py);
     sleep = abs(gRandom->Gaus(10,5));
     //sleep after every events to simulate the reconstruction time... 
     std::this_thread::sleep_for(std::chrono::seconds(sleep));
     reco_time=sleep;
     tree->Fill();
      //at the end of the event loop...put the sync function
      //************START OF SYNCING IMPLEMENTATION FROM USERS' SIDE**********************
     if((i+1)%sync_rate==0){
	    newfile->Sync(); //this one as a worker...
	    tree->Reset();
	      }

   }
   //do the syncing one more time
   if(tot_entries%sync_rate!=0)newfile->Sync(); 
   //************END OF SYNCING IMPLEMENTATION FROM USERS' SIDE***********************
 }
 newfile->MPIClose();
}
#ifndef __CINT__
int main(int argc,char* argv[]){
  int rank,size;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  test_tmpi();
  MPI_Finalize();
  return 0;
}
#endif
