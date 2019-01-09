# TMPI
This project builds a TFile-like object that uses MPI (Message Passing Interface) libraries and TMemFile for parallelization of IO process. 

## Author: Amit Bashyal 
    August 8, 2018

## PRE-REQUISITES
[ROOT](https://root.cern.ch/) preferably ROOT 6 or higher

[MPICH](http://www.mpich.org/) or other distributions like OpenMPI, Intel MPI, etc.

[CMake](https://cmake.org/) 3.2 or higher

## INSTALLATION
The following instructions assume the user has already built/installed ROOT, MPICH, and CMake in the machine.

Create a new working directory for the project **tmpi**:
```bash
mkdir tmpi
```
Put the TMPIFile repo under **tmpi**:
```bash
cd tmpi
git clone git@github.com:hep-cce/TMPIFile.git
```

Create a **build** and an **install** directory:
```bash
mkdir install build
```

**Make sure to enable dynamic linking on Cray system before build:**
```bash
export CRAYPE_LINK_TYPE=dynamic
```

Go to the **build** directory and build CMake:
```bash
cd build
C=mpicc CXX=mpicxx cmake ../TMPIFile/ -DCMAKE_INSTALL_PREFIX=/PATH/TO/tmpi/install/ -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

Make:
```bash
make install 
```

## USAGE EXAMPLE
Setup environment:
```bash
source install/env_tmpi.sh
```

Run example code:
```bash
mpirun -np 10 ./install/bin/test_tmpi
```

## CREDITS:
I would like to thank Taylor Childers for advising me, HEPCCE (High Energy Physics Center of Computational Excellence) program and Argonne National Laboratory for providing the opportunity to work on this project.
