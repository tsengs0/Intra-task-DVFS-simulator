#!/bin/bash

#in_alpha=$1

mkdir ../result/csv &&
mkdir ../result/identifier
 
for in_alpha in {65..100..5};
do
  for i in {0..49..1}
  do
	../bin/mul $in_alpha 1000 > log.txt
  done
  mkdir ../result/identifier/Alpha_$in_alpha &&
  mkdir ../result/csv/Alpha_$in_alpha &&
  mv *.txt ../result/identifier/Alpha_$in_alpha &&
  mv *.csv ../result/csv/Alpha_$in_alpha
done

