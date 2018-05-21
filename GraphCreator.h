/* 
 * File:   GraphCreator.h
 * Author: juanpablo
 *
 * Created on June 10, 2017, 2:13 AM
 */

#ifndef GRAPHCREATOR_H
#define GRAPHCREATOR_H
#include <iostream>
#include "stdafx.h"
#include "Graph.h"
#include "FileReader.h"
using namespace std;

struct GraphCreator
{

public:
	GraphCreator();

	Graph * createGraph(string path);

private:
	int numberLinesRead;
	FileReader fr;
	string line;
	int numberActivity;
	bool interpretLine(string line, Graph * graph);
	void readNumberOfActivities(string line, Graph * graph);
	void readDurationActivity(string line, Graph * graph, int numberActivity);
	bool readRelation(string line, Graph * graph);
	void labelActivities(Graph * graph);

};

#endif