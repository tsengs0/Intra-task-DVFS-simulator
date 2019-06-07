#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "No arguments supplied, please give the Experiment#"
    exit 1
fi

for in_alpha in 0 1 2 3 4; # from Tsk_0 to Tsk_4
do
	./csv_read NonDVFS $in_alpha $1
	./csv_read DVFS.NonOverhead $in_alpha $1
	./csv_read DVFS.Overhead $in_alpha $1
	#./csv_read DVFS.AlphaBound $in_alpha $1
	#./csv_read DVFS.AvgResp $in_alpha $1
done

mv *.csv ../eval
