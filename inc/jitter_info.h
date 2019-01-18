#ifndef __JITTER_INFO_H
#define __JITTER_INFO_H

typedef struct Jitter_constraint {
	double str_jitter_bound; // The margin of start time jitter
	double exe_jitter_bound; // The margin of execution time jitter
	double fin_jitter_bound; // The margin of finishing time jitter
	double alpha; // A percentage ranging from 0~100
	double fin_time_target;
} jitter_constraint;

#endif // __JITTER_INFO_H
