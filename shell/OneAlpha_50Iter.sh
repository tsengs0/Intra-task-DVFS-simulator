#!/bin/bash

in_alpha=$1

if [ $# -eq 0 ]
  then
    echo "No arguments supplied, please give the alpha value"
    exit 1
fi

 
  for i in {0..49..1}
  do
	../bin/mul $in_alpha 1000 #> log.txt
	echo $in_alpha
  done

