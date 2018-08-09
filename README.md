# TMPI
This project builds a TFile like object that uses MPICH (Message Passing Interface) libraries and TMemFile for parallelization of IO process. 

# Author: Amit Bashyal 
    August 8,2018

# PRE-REQUISITES
1. ROOT (preferably ROOT 6 or higher)
(https://root.cern.ch/) for download and installation instructions

2. MPICH
(http://www.mpich.org/) for download and installation instructions

# INSTALLATION
The following instructions assume the user has already build/installed
ROOT and MPICH in the machine.
1. Create a new working directory for the project (workdir)

2. In the workdir, open the "setenv.sh" file.

3. Replace "source <path/to/ROOTSYS/bin/thisroot.sh" with the actual path.

4. Put path to MPICH header files in "MPIINCLUDES=" .

5. Put  path to MPICH libraries in "MPILIBS=".

6. Save setenv.sh.

7. In the workdir, do "make".

# USAGE EXAMPLE
An example code (./src/test_tmpi.C) that shows the usage of the package

Example to run the macro with 10 parallel process:
In the workdir: "mpirun -np 10 ./bin/test_tmpi". 


# CREDITS:
I would like to thank Taylor Childers for adivising me, HEPCCE (High Energy Physics Center of Computational Excellence) 
program and Argonne National Laboratory for providing the opportunity to work on 
this project.
