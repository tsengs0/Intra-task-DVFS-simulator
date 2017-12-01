#!/bin/bash

trial_num=$1

if [ $# -eq 0 ]
  then
    echo "No arguments supplied, please give the Trial#"
    exit 1
fi

mkdir Total_csv_$trial_num &&
mv *.csv Total_csv_$trial_num &&
scp -r Total_csv_$trial_num s1610105@tlab21:~/Documents/research/Practices/Experimental_Result 
