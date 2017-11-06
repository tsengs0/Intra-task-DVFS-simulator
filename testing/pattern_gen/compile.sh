#!/bin/bash

g++ -std=c++11 -g -o main main.cpp cfg_info.* checkpoint_info.h dvfs_info.h jitter_info.h parser.* pattern_gen.* timer.*
