#bash script to setup environment for the makefile....

#setup the environment
module swap PrgEnv-intel PrgEnv-gnu
module load root/6.12.06
source $ROOT_DIR/bin/thisroot.sh

export MPIINCLUDES=$MPICH_DIR/include
export MPLIBS=$MPICH_DIR/lib
CURDIR=$(pwd)

export MPIINCLUDES
export MPLIBS
export CURDIR
export LD_LIBRARY_PATH=$CURDIR/lib:$LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
