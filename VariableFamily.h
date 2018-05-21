/* 
 * File:   VariableFamily.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:06 AM
 */

#ifndef VARIABLEFAMILY_H
#define VARIABLEFAMILY_H

#include <iostream>
#include "Variable.h"
#include <vector>

using namespace std;

class VariableFamily
{
public:
	VariableFamily(int index);
	void turnOnVariable(int varIndex);
	void turnOffVariable(int varIndex);
	void clear();
	bool isCorrect();
	void insertVariable(Variable * var);
	int getNumberVariables();
	Variable * getVariable(int indexVariable);
	int getAssocActivityNumber();
	bool hasActiveVariable();
	int getActiveVariable();
	int getIndex();
        void print();
        string getRestriccion();
        
private:
	vector<Variable *> variables;
	int numVariables;
	int currentOnVar;
	int index;
};

#endif