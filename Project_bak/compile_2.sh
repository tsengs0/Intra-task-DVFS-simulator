#!/bin/bash

clear

input_1=$1
run_1="run"
debug_1="debug"
debug_2="debug-run"
debug_3="gdb"

if [ "$input_1" == "$run_1" ] 
 then
	g++ -std=c++11 -o test src/main.cpp inc/main.h src/cfg_info.cpp src/rt_simulator.cpp inc/cfg_info.h inc/dvfs_info.h inc/jitter_info.h inc/rt_simulator.h
	
	sed 's/alpha_global = 3/alpha_global = 4/g' src/main.cpp > src/main_1.cpp
	g++ -std=c++11 -o test1 src/main_1.cpp inc/main.h src/cfg_info.cpp src/rt_simulator.cpp inc/cfg_info.h inc/dvfs_info.h inc/jitter_info.h inc/rt_simulator.h
	
	sed 's/task4.traverse_spec_path(case_t, WORST, release_time, start_time/task4.traverse_spec_path(case_t, WORST, release_time, start_time - (float) start_offset/g' src/main.cpp > src/main_2.cpp
	g++ -std=c++11 -o test2 src/main_2.cpp inc/main.h src/cfg_info.cpp src/rt_simulator.cpp inc/cfg_info.h inc/dvfs_info.h inc/jitter_info.h inc/rt_simulator.h
	
	sed 's/alpha_global = 3/alpha_global = 4/g' src/main_2.cpp > src/main_3.cpp
	g++ -std=c++11 -o test3 src/main_3.cpp inc/main.h src/cfg_info.cpp src/rt_simulator.cpp inc/cfg_info.h inc/dvfs_info.h inc/jitter_info.h inc/rt_simulator.h

	rm src/main_1.cpp src/main_2.c src/main_3.cpp
elif [ "$input_1" == "$debug_1" ] 
 then
	g++ -std=c++11 -g  src/main.cpp inc/main.h src/cfg_info.cpp src/rt_simulator.cpp inc/cfg_info.h inc/dvfs_info.h inc/jitter_info.h inc/rt_simulator.h
elif [ "$input_1" == "$debug_2" ] 
 then
	valgrind --leak-check=full --show-leak-kinds=all ./a.out task2.cfg 50 1000
elif [ "$input_1" == "$debug_3" ] 
 then
	valgrind --vgdb=yes --vgdb-error=0 ./a.out task2.cfg 15 1000
else
 	echo "There is no available option called $input_1"
fi

#mv *.gch obj/
