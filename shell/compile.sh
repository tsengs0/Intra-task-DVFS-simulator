#!/bin/bash

clear

input_1=$1
run_1="run"
run_2="mul"
debug_1="debug"
debug_2="debug-run"
debug_3="gdb"

if [ "$input_1" == "$run_1" ] 
 then
	g++ -std=c++11 -o test ../src/main.cpp \
				../inc/main.h \
				../src/cfg_info.cpp \
				../src/rt_simulator.cpp \
				../src/timer.cpp \
				../src/sched.cpp \
				../src/tick_cfg.cpp \
				../inc/cfg_info.h \
				../inc/dvfs_info.h \
				../inc/jitter_info.h \
				../inc/rt_simulator.h \
				../inc/timer.h \
				../inc/sched.h
elif [ "$input_1" == "$run_2" ] 
 then
	g++ -std=c++11 -o mul ../src/main.cpp \
				../inc/main.h \
				../src/cfg_info.cpp \
				../src/rt_simulator.cpp \
				../src/inter_bus.cpp \
				../src/timer.cpp \
				../src/sched.cpp \
				../src/tick_cfg.cpp \
				../inc/cfg_info.h \
				../inc/dvfs_info.h \
				../inc/jitter_info.h \
				../inc/rt_simulator.h \
				../inc/timer.h inc/sched.h
elif [ "$input_1" == "$debug_1" ] 
 then
	g++ -std=c++11 -g  ../src/main.cpp \
				../inc/main.h \
				../src/cfg_info.cpp \
				../src/rt_simulator.cpp \
				../src/inter_bus.cpp \
				../src/timer.cpp \
				../src/sched.cpp \
				../src/tick_cfg.cpp \
				../inc/cfg_info.h \
				../inc/dvfs_info.h \
				../inc/jitter_info.h \
				../inc/rt_simulator.h \
				../inc/timer.h inc/sched.h
elif [ "$input_1" == "$debug_2" ] 
 then
	valgrind --leak-check=full --show-leak-kinds=all ../bin/mul task2.cfg 50 1000
elif [ "$input_1" == "$debug_3" ] 
 then
	valgrind --vgdb=yes --vgdb-error=0 bin/mul task2.cfg 15 1000
else
 	echo "There is no available option called $input_1"
fi

#mv *.gch obj/
