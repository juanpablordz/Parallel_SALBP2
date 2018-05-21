/* 
 * File:   Equation.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:05 AM
 */

#ifndef	EQUATION_H
#define EQUATION_H

#include <iostream>
#include "Variable.h"
#include "Constant.h"
#include "Station.h"
#include <vector>

using namespace std;

class Equation
{
public:
	Equation();
	void insertVariable(Variable * var);
	void setConstant(Constant * c);
	void setAssocStation(Station * station);
	double getValue();
	double getConstantValue();
	vector<int> getSetActivitiesIndexes();
        int getNumVariables();
        Variable * getVariable(int i);
        
        void print();
private:
	vector<Variable *> variables;
	Constant * constant;
	Station * assocStation;
	int numVariables;
};

#endif