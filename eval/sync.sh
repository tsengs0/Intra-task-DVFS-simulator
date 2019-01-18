#!/bin/bash

trial_num=$1

if [ $# -eq 0 ]
  then
    echo "No arguments supplied, please give the Trial#"
    exit 1
fi

mkdir Total_csv_$trial_num &&
mv *.csv Total_csv_$trial_num 
scp -r Total_csv_$trial_num s1610105@tlab21:~/Documents/research/Master/Practices/Experimental_Result 
#&&  
#scp ShowExperiment_$trial_num.m s1610105@tlab21:~/Documents/research/Master/Practices/Experimental_Result 
#&&  
#scp -r Total_csv_$trial_num tsengs0@150.65.184.113:~/temp_program/Intra-task_DVFS/eval  
