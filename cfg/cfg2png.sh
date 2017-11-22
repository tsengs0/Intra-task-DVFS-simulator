#!/bin/bash

clear

dot_file=$1

dot -Tpng $dot_file.dot > $dot_file.png
