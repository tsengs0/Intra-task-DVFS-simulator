#ifndef __JITTER_INFO_H
#define __JITTER_INFO_H

typedef struct Jitter_constraint {
	float str_jitter_bound; // The margin of start time jitter
	float exe_jitter_bound; // The margin of execution time jitter
	float fin_jitter_bound; // The margin of finishing time jitter
	float alpha; // A percentage ranging from 0~100
	float fin_time_target;
} jitter_constraint;

#endif // __JITTER_INFO_H
