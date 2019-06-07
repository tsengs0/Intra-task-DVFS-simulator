#!/bin/bash

for in_alpha in {40..100..5};
do

	alpha_tsk0=$in_alpha
	alpha_tsk1=100
	alpha_tsk2=100
	alpha_tsk3=100
	alpha_tsk4=$(( 100 - alpha_tsk0 + 40 ))

	echo "Tsk0: " $alpha_tsk0 ", Tsk4: " $alpha_tsk4
done
