/* 
 * File:   InfoVariableAssignation.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:07 AM
 */

#ifndef INFOVARIABLEASSIGNATION_H
#define INFOVARIABLEASSIGNATION_H

//#define MAX_DEFAULT - DBL_MAX

#include <iostream>
#include <vector>
#include "CoordinatesVariable.h"

using namespace std;

class InfoVariableAssignation
{
public:
	InfoVariableAssignation();
	void updateValues(vector<double> * values, CoordinatesVariable * coordinates);
	CoordinatesVariable * getCoordinatesAssignationMaxMax();
	void flush();

private:
	int numberEquations;
	
	// Attempt to reduce memory inrease to zero
	double maxValue;
	int maxVarIndex;
	int maxVarFamIndex;

	void updateMax(vector<double> * values, CoordinatesVariable * coordinates);

};

#endif