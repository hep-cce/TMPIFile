#!/bin/bash
#COBALT -n 128
#COBALT -A datascience
#COBALT -t 30
#COBALT -q default

BASEPATH=/home/parton/git/TMPIFile/install

#setup the environment
module swap PrgEnv-intel PrgEnv-gnu
module load cernroot
source $ROOTSYS/bin/thisroot.sh

export LD_LIBRARY_PATH=$BASEPATH/lib:$LD_LIBRARY_PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH

# RANKS_PER_NODE=128
# NODES=1
# TOTAL_RANKS=$(( $NODES * $RANKS_PER_NODE ))
# THREADS_PER_CORE=2
# THREADS_PER_RANK=1

# aprun -n $TOTAL_RANKS -N $RANKS_PER_NODE -cc depth -d $THREADS_PER_RANK -j $THREADS_PER_CORE \
#    $BASEPATH/bin/test_tmpi > $COBALT_JOBID.${RANKS_PER_NODE}r_${NODES}n_1c.out 2>&1 &

RPN=8
NODES=128
COLLECTORS=8
TOTAL_RANKS=$(( $NODES * $RPN))
aprun -n $TOTAL_RANKS  -N $RPN  --cc none $BASEPATH/bin/test_tmpi -c $COLLECTORS > $COBALT_JOBID.${RPN}r_${NODES}n_${COLLECTORS}c.out 2>&1 &

wait

RPN=16
NODES=128
COLLECTORS=16
TOTAL_RANKS=$(( $NODES * $RPN))
aprun -n $TOTAL_RANKS  -N $RPN  --cc none $BASEPATH/bin/test_tmpi -c $COLLECTORS > $COBALT_JOBID.${RPN}r_${NODES}n_${COLLECTORS}c.out 2>&1 &

wait


RPN=32
NODES=128
COLLECTORS=32
TOTAL_RANKS=$(( $NODES * $RPN))
aprun -n $TOTAL_RANKS  -N $RPN  --cc none $BASEPATH/bin/test_tmpi -c $COLLECTORS > $COBALT_JOBID.${RPN}r_${NODES}n_${COLLECTORS}c.out 2>&1 &

wait




