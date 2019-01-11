#bash script to setup environment for the makefile....

#setup the environment
module swap PrgEnv-intel PrgEnv-gnu # on Cori & Theta
# module load root/6.12.06 # on Cori
module load cernroot # on Theta 
source $ROOTSYS/bin/thisroot.sh

# on Cray systems MPICH_DIR points at MPI libs/include
export MPIINCLUDES=$MPICH_DIR/include
export MPLIBS=$MPICH_DIR/lib
CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

export MPIINCLUDES
export MPLIBS
export CURDIR
export LD_LIBRARY_PATH=$CURDIR/lib:$LD_LIBRARY_PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
