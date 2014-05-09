#!/bin/sh

WINDOWS="3"
NPROCS="3"
FILTERS="mean"

for FILTER in $FILTERS ; do 
   for WSIZE in $WINDOWS ; do
     for NPROC in $NPROCS ; do 
       OUT=moon_f${FILTER}_w${WSIZE}_n${NPROCS}.tiff
       mpirun -np ${NPROC} /home/rohit/Desktop/homework3/src/pfilter input/moon_in.tiff ${OUT} ${FILTER} ${WSIZE} p_.tiff s_.tiff
     done
   done
done
 
