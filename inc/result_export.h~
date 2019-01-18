#ifndef __RESULT_EXPORT_H
#define __RESULT_EXPORT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <iterator>
#include <string>
#include <algorithm>
 
/*
 * A class to create and write data in a csv file.
 */
class CSVWriter
{
	std::string fileName;
	std::string delimeter;
	int linesCount;
 
public:
	CSVWriter(std::string delm = ",") :
			delimeter(delm), linesCount(0)
	{}

	void ExportFile_config(char *filename);
	/*
	 * Member function to store a range as comma seperated value
	 */
	//template<typename T>
	//void addDatainRow(T first, T last);
	void addDatainRow(float *first, float *last);
};

#endif // __RESULT_EXPORT_H
