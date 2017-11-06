#ifndef __DVFS_INFO_H
#define __DVFS_INFO_H

//#define PROCESSOR_1
#define PROCESSOR_AM335x
#define DISCRETE_DVFS
//#define CONTINUOUS_DVFS
#define DVFS_EN

#ifdef PROCESSOR_1
	#define FREQ_CNT      4    // Four discrete set
	#define MAX_speed     1.0    // 1k Hz
	#define MIN_speed     0.25 // 0.25k Hz
	static float freq_vol[][2] = {
		{0.250, 2000},  // 0.250k Hz, 2000 mV
		{0.500, 3000},  // 0.500k Hz, 3000 mV
		{0.750, 4000},  // 0.750k Hz, 4000 mV
		{1.000, 5000}   // 1k Hz, 5000 mV
	};
#endif

#ifdef  PROCESSOR_AM335x
	#define FREQ_CNT      5    // Five discrete set(MPU OPP)
	#define MAX_speed     1000    // 1000 MHz
	#define MIN_speed     300 // 300 MHz
	enum { // The id of MPU OPP
		OPP50  = 0,
		OPP100 = 1,
		OPP120 = 2,
		TURBO  = 3,
		NITRO  = 4
	};
	static float freq_vol[][2] = {
		{300 , 950},  // 300 MHz ,  950 mV
		{600 , 1100}, // 600 MHz , 1100 mV
		{720 , 1200}, // 720 MHz , 1200 mV
		{800 , 1260}, // 800 MHz , 1260 mV
		{1000, 1325} // 1000 MHz, 1325 mV 
	};
	static float MPU_POWER[] = {
		114.38, // 114.38 mW
		303.15, // 303.15 mW
		437.49, // 437.49 mW
		542.73, // 542.73 mW
		736.08  // 736.08 mW
	};
#endif

#endif // __DVFS_INFO_H
