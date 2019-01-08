# TMPI
This project builds a TFile-like object that uses MPI (Message Passing Interface) libraries and TMemFile for parallelization of IO process. 

# Author: Amit Bashyal 
    August 8, 2018

# PRE-REQUISITES
    1. [ROOT](https://root.cern.ch/) (preferably ROOT 6 or higher) a data processing, statistical analysis, visualization and storage framework

    2. [MPICH](http://www.mpich.org/) or any other MPI (e.g. OpenMPI, Intel MPI, etc.) distributions

    3. [CMake](https://cmake.org/) a cross-platform compilation tool

# INSTALLATION
    The following instructions assume the user has already built/installed
    ROOT, MPICH, and CMake in the machine.

    1. Create a new working directory for the project (tmpi)
    ```bash
    mkdir tmpi
    ```

    2. Put the TMPIFile repo under "tmpi".
    ```bash
    cd tmpi
    git clone git@github.com:hep-cce/TMPIFile.git
    ```

    3. Create a "build" and an "install" directory
    ```bash
    mkdir install build
    ```

    4. Go to the "build" directory and build CMake
    ```bash
    cd build
    C=mpicc CXX=mpicxx cmake ../TMPIFile/ -DCMAKE_INSTALL_PREFIX=/PATH/TO/tmpi/install/ -DCMAKE_BUILD_TYPE=RelWithDebInfo
    ```

    5. Make
    ```bash
    make install 
    ```

# USAGE EXAMPLE

    1. Setup environment
    ```bash
    source install/env_tmpi.sh
    ```

    2. Run example code (in the "tmpi")
    ```bash
    mpirun -np 10 ./install/bin/test_tmpi
    ```

# CREDITS:
    I would like to thank Taylor Childers for advising me, HEPCCE (High Energy Physics Center of Computational Excellence) 
    program and Argonne National Laboratory for providing the opportunity to work on 
    this project.
