#!/bin/bash

 
for a in {40..100..5}
do
	mkdir ../temp_result/$a
done

for in_alpha in {40..100..5}
do
  for i in {0..2..1}
  do
	../bin/mul $in_alpha 100 100 100 100 1000 > log.txt
	echo $in_alpha
	echo $i
  	mkdir ../temp_result/$in_alpha/$i &&
  	cp *.csv *.txt ../temp_result/$in_alpha/$i &&
  	rm *.csv *.txt
  done
done
