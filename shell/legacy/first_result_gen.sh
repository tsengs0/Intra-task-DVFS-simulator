#!/bin/bash

#in_alpha=$1

if [ $# -eq 0 ]
  then
    echo "No arguments supplied, please give the Trial#"
    exit 1
fi

mkdir ../result/csv/Experiment_$1 &&
mkdir ../result/identifier/Experiment_$1
 
for in_alpha in {20..100..2};
do
  for i in {0..49..1}
  do
	../bin/mul $in_alpha 1000 > log.txt
	echo $in_alpha
  done
  mkdir ../result/identifier/Experiment_$1/Alpha_$in_alpha &&
  mkdir ../result/csv/Experiment_$1/Alpha_$in_alpha &&
  mv *.txt ../result/identifier/Experiment_$1/Alpha_$in_alpha &&
  mv *.csv ../result/csv/Experiment_$1/Alpha_$in_alpha
done

