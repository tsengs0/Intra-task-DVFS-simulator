#!/bin/bash

 
for a in {40..100..5}
do
	mkdir ../temp_result/$a
done

for in_alpha in {40..100..5}
do
	alpha_tsk0=$in_alpha
	alpha_tsk4=$(( 100 - alpha_tsk0 ))
	echo "alpha_tsk0 = " $alpha_tsk0
	echo "alpha_tsk1 = " $alpha_tsk4
  for i in {0..9..1}
  do
	../bin/mul $alpha_tsk0 100 100 100 $alpha_tsk4 1000 > log.txt
	echo $in_alpha
	echo $i
  	mkdir ../temp_result/$in_alpha/$i &&
  	cp *.csv *.txt ../temp_result/$in_alpha/$i &&
 	rm *.csv *.txt
  done
done
