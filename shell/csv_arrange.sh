#!/bin/bash

for in_alpha in 0 1 2;
do
	./csv_read NonDVFS $in_alpha
echo
echo
echo
	./csv_read DVFS.NonOverhead $in_alpha
echo
echo
echo
	./csv_read DVFS.Overhead $in_alpha
echo
echo
echo
done

mv *.csv ../eval
