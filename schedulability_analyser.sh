#!/bin/bash

g++ -std=c++11 -o sched_analyser main.cpp src/timer.cpp src/sched.cpp inc/timer.h inc/dvfs_info.h inc/sched.h inc/main.h
