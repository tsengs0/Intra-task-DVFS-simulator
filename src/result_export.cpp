#include "../inc/result_export.h"

/*
 * This Function accepts a range and appends all the elements in the range
 * to the last row, seperated by delimeter (Default is comma)
 */
//template<typename T>
//void CSVWriter::addDatainRow(T first, T last)
void CSVWriter::addDatainRow(float *first, float *last)
{
	std::fstream file;
	// Open the file in truncate mode if first line else in Append Mode
	file.open(fileName, std::ios::out | (linesCount ? std::ios::app : std::ios::trunc)); 
	
	// Iterate over the range and add each lement to file seperated by delimeter.
	for (; first != last; )
	{
		file << *first;
		if (++first != last)
			file << delimeter;
	}
	file << "\n";
	linesCount++;
 
	// Close the file
	//file.close();
}

void CSVWriter::ExportFile_config(char *filename)
{
        std::string temp(filename);
        fileName = temp;
}
/*
int main()
{
	// Creating an object of CSVWriter
	CSVWriter writer("example.csv");
 
	std::vector<std::string> dataList_1 = { "20", "hi", "99" };
 
	// Adding vector to CSV File
	writer.addDatainRow(dataList_1.begin(), dataList_1.end());
 
	// Create a set of integers
	std::set<int> dataList_2 = { 3, 4, 5};
 
	// Adding Set to CSV File
	writer.addDatainRow(dataList_2.begin(), dataList_2.end());
 
	std::string str = "abc";
 
	// Adding characters in a string in csv file.
	writer.addDatainRow(str.begin(), str.end());
 
	// An array of int
	int arr [] = {3,4,1};
 
	// Wrote an elements in array to csv file.
	writer.addDatainRow(arr , arr + sizeof(arr) / sizeof(int));
 
	int in[3][3] = {
		{1,2,3},
		{4,5,6},
		{7,8,9}
	};
	for(int i = 0; i < 3; i++) writer.addDatainRow(in[i], in[i] + sizeof(in[i]) / sizeof(int));

	return 0;
}
*/
