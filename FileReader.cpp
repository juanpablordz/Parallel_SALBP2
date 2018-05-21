/* 
 * File:   FileReader.cpp
 * Author: juanpablo
 * 
 * Created on June 10, 2017, 2:14 AM
 */

#include <iostream>
#include "stdafx.h"
#include <stdio.h>
#include <string>
#include "FileReader.h"
//#include "strtk.hpp"
// #include <tchar.h>

FileReader::FileReader()
{
	path="";
}

FileReader::FileReader(string path)
{
	FileReader::path=path;
}

void FileReader::openFile()
{
	infile.open(path);
}

void FileReader::closeFile()
{
	infile.close();
}

bool FileReader::isEOF() const
{
	//return infile.eof;
	return true;
}

string FileReader::getLine()
{
	getline(infile,currLine);
	return currLine;
}
// Uso de str tokenizer


void FileReader::parseTwoValues(string line, int * a, int * b)
{
        //buscar una funcion parse que me pueda resolver esto, o tokenizer
	std::vector<int> list;
        int var1 = atoi(line.substr(0,line.find(",")).c_str());
        int var2 = atoi(line.substr(line.find(",")+1,line.size()).c_str());
       
        list.push_back(var1);
        list.push_back(var2);
        
        
	
	if(list.size()<=2)
	{
		*a=list[0];
		*b=list[1];
                
	}
	else
	{
		throw ("Error: Arguments number must equal to two.");
	}
}
 