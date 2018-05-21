/* 
 * File:   AssemblyLine.h
 * Author: juanpablo
 *
 * Created on June 10, 2017, 2:12 AM
 */

#ifndef ASSEMBLYLINE_H
#define ASSEMBLYLINE_H

#include <iostream>
#include <vector>
#include "Station.h"
using namespace std;

class AssemblyLine
{
public:
	AssemblyLine(int numberStations);

	int getNumberStations();
	Station * getStation(int i);
	void printAssemblyLine();
	double getMaxTime();
	vector<vector<int>> getAssignationIndexes();
private:
	int numberStations;
	double maxTimeStation;
	vector<Station *> stations;

	
};

#endif
