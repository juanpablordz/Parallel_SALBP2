/* 
 * File:   VariableFamily.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:06 AM
 */

#include <iostream>
#include "stdafx.h"
#include "VariableFamily.h"
#include "Variable.h"

VariableFamily::VariableFamily(int index)
{
	numVariables=0;
	currentOnVar=-1;
	VariableFamily::index=index;
}

int VariableFamily::getIndex()
{
	return index;
}

void VariableFamily::turnOnVariable(int varIndex)
{
	if(currentOnVar!=-1)
	{
		variables[currentOnVar]->setFlag(false);
	}
	currentOnVar=varIndex;
	variables[currentOnVar]->setFlag(true);
}

void VariableFamily::turnOffVariable(int varIndex)
{
	if(currentOnVar!=varIndex)
	{
		throw ("Something went wrong in turning off var.");
		
	}
	variables[currentOnVar]->setFlag(false);
	currentOnVar=-1;
}

void VariableFamily::clear()
{
	for(int i=0;i<variables.size();i++)
	{
		variables[i]->setFlag(false);
	}
	currentOnVar=-1;
}

bool VariableFamily::isCorrect()
{
	int numOn=0;
	for(int i=0;i<numVariables;i++)
	{
		if(variables[i]->isOn())
		{
			numOn++;
		}
	}
	return numOn<=1;
}

void VariableFamily::insertVariable(Variable * var)
{
	variables.push_back(var);
	numVariables++;
}

int VariableFamily::getNumberVariables()
{
	return numVariables;
}

Variable * VariableFamily::getVariable(int indexVariable)
{
	return variables[indexVariable];
}

int VariableFamily::getAssocActivityNumber()
{
	if(numVariables>0)
	{
		return variables[0]->getActivityNumber();
	}
	return -1;
}

bool VariableFamily::hasActiveVariable()
{
	return currentOnVar>=0;
}

int VariableFamily::getActiveVariable()
{
	return currentOnVar;
}

void VariableFamily::print()
{
    for(int i=0; i<numVariables;i++){
        
        variables[i]->print();
        //solo poner ":" cuando no es el ultimo elemento.
        if(i != numVariables-1)
            cout <<":";
        
    }
    
    // cambiar de renglon para las de unicidad
    cout << endl;
}


string VariableFamily::getRestriccion()
{
    string str;
    
    for(int i=0; i<numVariables;i++){
        str += variables[i]->getActSta();
        //solo poner ":" cuando no es el ultimo elemento.
        if(i != numVariables-1)
            str += ",";
    }
    
    return str;
    
}
