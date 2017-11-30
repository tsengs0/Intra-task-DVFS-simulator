#!/bin/bash

in_alpha=$1
#for in_alpha in {50..100};
#do
  for i in {0..49..1}
  do
	../bin/mul $in_alpha 1000 > log.txt
  done
  mkdir ../result/Alpha_$in_alpha &&
  mv *.txt ../result/Alpha_$in_alpha
#done

