/* 
 * File:   FileReader.h
 * Author: juanpablo
 *
 * Created on June 10, 2017, 2:14 AM
 */

#ifndef FILEREADER_H
#define FILEREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct FileReader
{
public:
	string path;
	ifstream infile;
	string currLine;

	FileReader();
	FileReader(string path);

	void openFile();
	void closeFile();
	bool isEOF() const;
	string getLine();
	void parseTwoValues(string line, int * a, int * b);

};

#endif