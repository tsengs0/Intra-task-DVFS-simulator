#!/bin/bash

for i in {0..100..5}
do
 for a in 250 500 750 1000
 do
	./test $1 $i $a
 done
done

echo 

mkdir $1_results &&
mv test_result* $1_results/
