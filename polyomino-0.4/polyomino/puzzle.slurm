#!/bin/bash
#SBATCH -A puzzlepoesis
#SBATCH -p standard #partition/queue name
#SBATCH -N 1 # 1 node
#SBATCH -n 1 # 1 task
#SBATCH -t 7-00:00:00 # time limit dd-hh:mm:ss
#SBATCH --mem=6000 # 6000MB of total memory, max 120000 in standard partition

module purge #remove any loaded modules as a precaution
OUTPUTDIR=/scratch/$USER/poly-$SLURM_JOBID      #  create unique path using job id. $USER avoids hardcoding your computing id 
mkdir -p $OUTPUTDIR     # create the output directory

./tetr_pentomino < poesis.in > $OUTPUTDIR/poly-out.txt
