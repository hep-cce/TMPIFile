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

#include "mpi.h"

#include <chrono>
#include <random>
#include <thread>
#include <string>
#include <iostream>
#include <sys/wait.h>

using namespace std;

void SharedWriter(TMPIFile *newfile) {
    // Number of events per worker
    Int_t N_events = 12;
    Int_t N_workers = 67; // number of workers per shared writer
    Int_t sync_rate = 16; // events per send request by the shared writer

    // The size of each event's output data is between 2 and 3 MB
    uniform_real_distribution<double> dist_N(262144., 393216.);
    // Random doubles [0., 100.] to fill output data
    uniform_real_distribution<double> dist(0., 100.);
    // Event processing time: 2mins +/- 30secs
    uniform_real_distribution<double> dist_time(3., 10.);
    uniform_real_distribution<double> dist_t(0., 2.);
    random_device rd;
    mt19937 mt(rd());

    Int_t temp_num = 0;
            
    TTree *tree = new TTree("tree","tree");
    tree->SetAutoFlush(400000000);
    
    TH1D *h = new TH1D("hist", "hist", 100, 0, 100);
    Float_t px,py;
    
    tree->Branch("px", &px);
    tree->Branch("py", &py);
    tree->Branch("histogram", h);

    for (int i = 0; i < N_events; i++) {
        int time = 90;//(int) dist_time(mt);
        int temp_time = 0;
        sleep(time);

        for (int j = 0; j < N_workers; j++) {
            const int N = dist_N(mt);
            
            px = dist(mt);
            py = dist(mt);
            for (int k = 0; k < N; k++) {
                h->Fill(dist(mt));
            }
            tree->Fill();
            h->Reset();
            temp_num++;

            int t = (int) dist_t(mt);
            temp_time += t;
            sleep(t);
            
            string msg = "ID: ";
            msg += to_string(j);
            msg += "\tEvent: ";
            msg += to_string(i);
            msg += "\tProcessing time: ";
            msg += to_string(time + temp_time);
            msg += "\tData: ";
            msg += to_string(N);
            msg += "\tcounter: ";
            msg += to_string(temp_num);
            Info("Worker", msg.c_str());
            //at the end of the event loop...put the sync function
            //************START OF SYNCING IMPLEMENTATION FROM USERS' SIDE**********************
            if (temp_num == sync_rate) {
                newfile->Sync(); //this one as a worker...
                temp_num = 0;
                tree->Reset();
            }
        }
    }
    //do the syncing one more time
    if((N_events+N_workers)%sync_rate!=0)
        newfile->Sync(); 
    delete h;
    delete tree;
    //************END OF SYNCING IMPLEMENTATION FROM USERS' SIDE***********************
    Info("Worker()", "All done");
}

void test_athena(){
    char mpifname[100];
    Int_t N_collectors = 16; // specify how many collectors to receive the message
    
    sprintf(mpifname,"Simple_MPIFile.root");
    TMPIFile *newfile = new TMPIFile("Simple_MPIFile.root","RECREATE",N_collectors);
    Int_t seed = newfile->GetMPILocalSize()+newfile->GetMPIColor()+newfile->GetMPILocalRank();
    
    //Start the Collector for each MPI world
    if (newfile->IsCollector()) newfile->RunCollector();
    // shared writers on each node fork multiple workers 
    else {
        string msg = "Shared writer rank: ";
        msg += to_string(newfile->GetMPILocalRank());
        msg += ", PID: ";
        msg += to_string(getpid());
        Info("Athena TMPI test", msg.c_str());

        // One shared writer per node to collect
        // data from multiple workers
        SharedWriter(newfile);
        Info("test_athena", "else done");
    }
    Info("test_athena", "to close");
    newfile->MPIClose();
    delete newfile;
    Info("test_athena", "ALL DONE");
}

#ifndef __CINT__
int main(int argc,char* argv[]){
    auto start = chrono::high_resolution_clock::now();
    
    int rank, size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    test_athena();
    MPI_Finalize();
    
    auto end = chrono::high_resolution_clock::now();
    double time = chrono::duration_cast<chrono::duration<double>> (end - start).count();
    string msg = "Total elapsed time: ";
    msg += to_string(time);
    msg += "\tTotal rank: ";
    msg += to_string(size);
    if (rank == 0)
        Info("Athena TMPI test", msg.c_str());
    
    return 0;
}
#endif
