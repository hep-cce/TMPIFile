
#bash script to setup environment for the makefile....

#setup the ROOT environment variables
source <path/to/ROOTSYS/bin/thisroot.sh

MPIINCLUDES= #path to {MPICH}/include/
MPLIBS= #path to {MPICH}/include/
CURDIR=$PWD

export MPIINCLUDES
export MPLIBS
export CURDIR
export LD_LIBRARY_PATH=$CURDIR/lib:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
