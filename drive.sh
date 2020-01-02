#!/bin/csh -f

#in order:  original file, defected file, output file name, skip text file name
set commands="$1 $2 $3 $4"
./datGen $commands

#in order:  input file name (same as output file name from last command), output figure file name (you should include .pdf at the end for best results)
set commands2="$3 $5"
python displacementGraph.py $commands2

