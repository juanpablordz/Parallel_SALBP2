/* 
 * File:   GroupFamilies.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:07 AM
 */

#ifndef GROUPFAMILIES_H
#define GROUPFAMILIES_H

#include <iostream>
#include "VariableFamily.h"
#include "CoordinatesVariable.h"
#include <vector>

using namespace std;

class GroupFamilies
{
public:
	GroupFamilies();
	void insertFamily(VariableFamily * fam);
	VariableFamily * getFamily(int famIndex);
	VariableFamily * getFamily(Activity * activity);
	int getNumberFamilies();
	void turnOnVariable(CoordinatesVariable * coordinates);
	void turnOffVariable(CoordinatesVariable * coordinates);
	void updateValues(vector<int> * variablesState);
        void print();
        vector<string> getArrRest();

private:
	vector<VariableFamily *> families;
	int numberFamilies;
};

#endif