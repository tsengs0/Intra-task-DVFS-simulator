// DataGenerator.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include "string"
#include <random>
#include <conio.h>
#include <iomanip>
#include <fstream>

using std::vector;

#define tasks_num 5
#define HP 3

bool schedulability(int TskNum, double util);
double util_bound(int num);
void adj_prt(double util_bound, int *period);
bool isHP(unsigned int TskID, int *period);

double Tsk_WCET[] = {
	11950,   // compress.c
	1810,     // cfg_1
	9750,    // bs.c
	27546,   // ludcmp.c
	1890395 // matmult.c 
};
typedef const char* tsk_name;
tsk_name TSK_NAME[5];
const char *tsk1 = "compress.c";
const char *tsk2 = "cfg_1";
const char *tsk3 = "bs.c";
const char *tsk4 = "ludcmp.c";
const char *tsk5 = "matmult.c";

vector<int> hp_tsk_temp;

int main()
{
	const int cap = 1; // capacity of the system (the number of processor)
	const int nintervals = tasks_num; // number of intervals (maximum number of tasks)

	for (int i = 0; i < tasks_num; i++) {
		if (i == 0) TSK_NAME[i] = tsk1;
		else if (i == 1) TSK_NAME[i] = tsk2;
		else if (i == 2) TSK_NAME[i] = tsk3;
		else if (i == 3) TSK_NAME[i] = tsk4;
		else if (i == 4) TSK_NAME[i] = tsk5;
	} 

	//Generate the Utilization
	std::default_random_engine generator; // a random generator
	std::exponential_distribution<double> utildis(3.5);
	double p[nintervals] = {}; // temperary utilization
	double util[5][nintervals]; // recode of utilization under RM scheduling
	double UtilTsk_n;
	int i;
	double num;
	double sum, temp_sum;

	UtilTsk_n = util_bound(nintervals);
	for (int k = 0; k < 5; k++) // k corresponds to each utilization interval: k=0 -> 60%, k=1 -> 70%, .. and so on.
	{
		i = 0;
		sum = 0;
		temp_sum = 0;
		while (i < nintervals) // i correspods to each task
		{
			num = utildis(generator);
			if (num >= 0.01 && num <= 0.15) // here we just limit the acceptable utilization among 0.1 to 0.3 in order to increase the number of tasks
			{                             // If we accept a larger utilization, then the number of tasks is just a few.
				num = ceil(num * 1000) / 1000; // For convenience of multiprocessor scheduling, the utilizations are expected to be rounded, example: 0.1, 0.2, ... and so on.
				temp_sum = sum + num;      // temporary sum of utilization
				if (temp_sum > UtilTsk_n) // if sum of utilization exceeds the expected total utilization (for example: 60% of processor capacity)
				{                                       // then the total utilization is set to the exepected one.
					p[i] = UtilTsk_n - sum;
					sum = UtilTsk_n;
					break;
				}
				p[i] = num;
				sum = temp_sum;
				if (sum == UtilTsk_n) // if the total utilization is equal to the expected one, then we stop generating tasks
					break;
				i++;
			}
		}

		// copy the temporary utilizations to the record of utilization
		for (int l = 0; l < nintervals; l++)
		{
			util[k][l] = p[l];
		}
	}
	// print the generated utilizations to the terminal screen
	std::cout << "Utilisation:  " << std::endl;
	for (int i = 0; i < 5; i++)
	{
		double temp = (double) 0.0;
		bool SchedVerify;
		std::cout << i << ":" << std::endl;
		for (int j = 0; j < nintervals; j++)
		{
			temp += util[i][j];
			std::cout << "Tsk_" << j << ": " << util[i][j] << "  ";
		}
		SchedVerify = schedulability(nintervals, temp);
		if (SchedVerify == true)
			std::cout << std::endl << "Utilisation: " << temp << "%(Bound:" << UtilTsk_n << ", schedulable!)" << std::endl << std::endl;
		else
			std::cout << std::endl << "Utilisation: " << temp << "%(Bound:" << UtilTsk_n << ", not schedulable!)" << std::endl << std::endl;
	}
	/*
	//Generate WCET
	std::uniform_int_distribution<int> wcetdis(1, 20); // Wcet is generated using an uniform distribution
	int wcet[5][nintervals];                           // We here limit the wcet to be integer numbers lower than 20
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < nintervals; j++)
		{
			if (util[i][j] > 0)
				wcet[i][j] = wcetdis(generator);
			else
				wcet[i][j] = 0;
		}
	}
	// print the generated wcet to the terminal screen
	std::cout << "WCET:  " << std::endl;
	for (int i = 0; i < 5; i++)
	{
		std::cout << 60 + i * 10 << "%:" << std::endl;
		std::cout << "1: ";
		for (int j = 0; j < nintervals; j++)
		{
			std::cout << wcet[i][j] << "  ";
		}
		std::cout << std::endl;
	}

	// Similarly to WCET, we can generate ET and period
	//Generate ET
	int et[5][nintervals];
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < nintervals; j++)
		{
			if (util[i][j] > 0)
			{
				std::uniform_int_distribution<int> etdis(1, wcet[i][j]);
				et[i][j] = etdis(generator);
			}
			else
				et[i][j] = 0;

		}
	}
	// print the generated ET to the terminal screen
	std::cout << "ET:  " << std::endl;
	for (int i = 0; i < 5; i++)
	{
		std::cout << 60 + i * 10 << "%:" << std::endl;
		std::cout << "1: ";
		for (int j = 0; j < nintervals; j++)
		{
			std::cout << et[i][j] << "  ";
		}
		std::cout << std::endl;
	}
	*/
	//Generate Period
	int period[5][nintervals];
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < nintervals; j++)
		{
			if (util[i][j] > 0)
				period[i][j] = std::ceil(Tsk_WCET[j] / util[i][j]);
			else
				period[i][j] = 0;
		}
		// Adjust all tasks' utilisation to match the user-defined highest priority requirement
		adj_prt(UtilTsk_n, period[i]);
	}
	// print the generated period to the terminal screen
	std::cout << "Period:  " << std::endl;
	for (int i = 0; i < 5; i++)
	{
		double temp = (double) 0.0;
		bool SchedVerify;
		std::cout << i << ":" << std::endl;
		for (int j = 0; j < nintervals; j++)
		{
			temp += Tsk_WCET[j] / period[i][j];
			if (j != nintervals - 1) {
				std::cout << TSK_NAME[j] << ": (" << Tsk_WCET[j] << " / " << period[i][j] << ")" << std::endl;
			}
			else {
				std::cout << TSK_NAME[j] << ": (" << Tsk_WCET[j] << " / " << period[i][j] << ")" << std::endl;
			}
		}

		SchedVerify = schedulability(nintervals, temp);
		if (SchedVerify == true)
			std::cout << std::endl << "Utilisation: " << temp << "%(Bound:" << UtilTsk_n << ", schedulable!)" << std::endl << std::endl;
		else
			std::cout << std::endl << "Utilisation: " << temp << "%(Bound:" << UtilTsk_n << ", not schedulable!)" << std::endl << std::endl;
		std::cout << std::endl;
	}

	/*
	// Create output data
	std::ofstream outputfile;
	outputfile.open("periodic.cfg.1-60");
	outputfile << "\Target Periodic Utilization = 60.00" << std::endl;
	outputfile << "\tid\twcet\tet\tprd\treq_tim" << std::endl;
	for (int i = 0; i < nintervals; i++)
	{
		if (util[0][i] > 0)
		{
			for (int k = 0; k*period[0][i] < 1000; k++)
			{
				outputfile << i << "\t" << wcet[0][i] << "\t" << et[0][i] << "\t" << period[0][i] << "\t" << k*period[0][i] << std::endl;
			}
		}
	}
	outputfile.close();

	outputfile.open("periodic.cfg.16-70");
	outputfile << "\Target Periodic Utilization = 70.00" << std::endl;
	outputfile << "\tid\twcet\tet\tprd\treq_tim" << std::endl;
	for (int i = 0; i < nintervals; i++)
	{
		if (util[1][i] > 0)
		{
			for (int k = 0; k*period[1][i] < 1000; k++)
			{
				outputfile << i << "\t" << wcet[1][i] << "\t" << et[1][i] << "\t" << period[1][i] << "\t" << k*period[1][i] << std::endl;
			}
		}
	}
	outputfile.close();

	outputfile.open("periodic.cfg.1-80");
	outputfile << "\Target Periodic Utilization = 80.00" << std::endl;
	outputfile << "\tid\twcet\tet\tprd\treq_tim" << std::endl;
	for (int i = 0; i < nintervals; i++)
	{
		if (util[2][i] > 0)
		{
			for (int k = 0; k*period[2][i] < 1000; k++)
			{
				outputfile << i << "\t" << wcet[2][i] << "\t" << et[2][i] << "\t" << period[2][i] << "\t" << k*period[2][i] << std::endl;
			}
		}
	}
	outputfile.close();
	outputfile.open("periodic.cfg.1-90");
	outputfile << "\Target Periodic Utilization = 90.00" << std::endl;
	outputfile << "\tid\twcet\tet\tprd\treq_tim" << std::endl;
	for (int i = 0; i < nintervals; i++)
	{
		if (util[3][i] > 0)
		{
			for (int k = 0; k*period[3][i] < 1000; k++)
			{
				outputfile << i << "\t" << wcet[3][i] << "\t" << et[3][i] << "\t" << period[3][i] << "\t" << k*period[3][i] << std::endl;
			}
		}
	}
	outputfile.close();
	outputfile.open("periodic.cfg.1-100");
	outputfile << "\#Target Periodic Utilization = 100.00" << std::endl;
	outputfile << "\#tid\twcet\tet\tprd\treq_tim" << std::endl;
	for (int i = 0; i < nintervals; i++)
	{
		if (util[4][i] > 0)
		{
			for (int k = 0; k*period[4][i] < 1000; k++)
			{
				outputfile << i << "\t" << wcet[4][i] << "\t" << et[4][i] << "\t" << period[4][i] << "\t" << k*period[4][i] << std::endl;
			}
		}
	}
	outputfile.close();
	*/

	std::cout << "end!" << std::endl;
	_getch();
	return 0;
}

/**
* @brief Executing/simulating the sub-portion of control flow graph in terms of
*	   interrupt timer's period(preloaded cycle count). For sake of simulating
*        preemptive behaiour
* @param case_t: designating the estimated 1)worst-case, 2)average-case,
*        3)best-case to entire execution
* @param DVFS_en: enable or disable DVFS mechanism
**/
double util_bound(int num)
{
	return num * (pow(2, (double)1 / num) - 1);
}

/**
* @brief Executing/simulating the sub-portion of control flow graph in terms of
*	   interrupt timer's period(preloaded cycle count). For sake of simulating
*        preemptive behaiour
* @param case_t: designating the estimated 1)worst-case, 2)average-case,
*        3)best-case to entire execution
* @param DVFS_en: enable or disable DVFS mechanism
**/
bool schedulability(int TskNum, double util)
{
	return (util <= util_bound(TskNum)) ? true : false;
}

void adj_prt(double util_bound, int *period)
{
	hp_tsk_temp.clear();
	while (isHP((unsigned int)HP, period) == false) {
		//std::cout << "-------------------------------------------" << std::endl;
		for (unsigned int i = 0; i < hp_tsk_temp.size(); i++) {
		while (period[hp_tsk_temp[i]] - 1 < period[HP]) {
				period[hp_tsk_temp[i]] += 1; 
				period[HP] = (period[HP] > Tsk_WCET[HP]) ? period[HP] - 1 : period[HP];
				//std::cout << "Tsk_" << hp_tsk_temp[i] << ".period: " << period[hp_tsk_temp[i]] << std::endl;
				//std::cout << "HP Tsk_" << HP << ".period: " << period[HP] << std::endl;
			}
			
			//std::cout << "Tsk[" << hp_tsk_temp[i] << "]" << std::endl;
		}
		//std::cout << "-------------------------------------------" << std::endl;
		hp_tsk_temp.clear();
	}

}

bool isHP(unsigned int TskID, int *period)
{
	bool is_hp = true;
	for (unsigned int i = 0; i < (unsigned int)tasks_num; i++)
	{
		if (period[i] < period[TskID] && i != TskID) {
			//std::cout << "     Period[" << i << "]: " << period[i] << std::endl << std::endl << "(HP) Period[" << TskID << "]: " << period[TskID] << std::endl << std::endl;
			hp_tsk_temp.push_back(i);
			is_hp = false; 
		}
	}
	return is_hp;
}

