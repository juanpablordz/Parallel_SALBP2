/* 
 * File:   Equation.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:05 AM
 */

#include <iostream>
#include "stdafx.h"
#include "Equation.h"
#include "Variable.h"

Equation::Equation()
{
	numVariables=0;
}

void Equation::insertVariable(Variable * var)
{
	variables.push_back(var);
	numVariables++;
}

void Equation::setConstant(Constant * c)
{
	Equation::constant=c;
}

double Equation::getValue()
{
	double suma=0;

	for(int i=0;i<numVariables;i++)
	{
		suma+=variables[i]->getValue();
	}
	suma+=constant->getValue();

	return suma;
}

void Equation::setAssocStation(Station * station)
{
	assocStation=station;
}

double Equation::getConstantValue()
{
	return constant->getValue();
}

vector<int> Equation::getSetActivitiesIndexes()
{
	vector<int> indexes;
	vector<Activity *> activites = constant->getAssocActivities();
	int i;

	for(i=0;i<activites.size();i++)
	{
		indexes.push_back(activites[i]->number);
	}
	for(i=0;i<variables.size();i++)
	{
		if(variables[i]->isOn())
		{
			indexes.push_back(variables[i]->getActivityNumber());	
		}
	}

	sort(indexes.begin(),indexes.end());
	return indexes;
}

void Equation::print()
{
    cout << "\n ***" << endl;
    cout << "Station: " << assocStation->number << endl;
    cout << "Constant: " << constant->getValue() << endl;
    
        for(int i=0; i<numVariables; i++){
        variables[i]->print();
    }
    
}

int Equation::getNumVariables()
{
    return numVariables;
}

Variable * Equation::getVariable(int i)
{
    return variables[i];
}