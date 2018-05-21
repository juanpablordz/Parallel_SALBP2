/* 
 * File:   GroupFamilies.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:07 AM
 */

#include <iostream>
#include "stdafx.h"
#include "GroupFamilies.h"
#include "VariableFamily.h"

GroupFamilies::GroupFamilies()
{
	numberFamilies=0;
}

void GroupFamilies::insertFamily(VariableFamily * fam)
{
	families.push_back(fam);
	numberFamilies++;
}

VariableFamily * GroupFamilies::getFamily(int famIndex)
{
	return families[famIndex];
}

VariableFamily * GroupFamilies::getFamily(Activity * activity)
{
	for(int i=0;i<numberFamilies;i++)
	{
		if(getFamily(i)->getAssocActivityNumber()== activity->number)
		{
			return getFamily(i);
		}
	}
	return NULL;
}

int GroupFamilies::getNumberFamilies()
{
	return numberFamilies;
}

void GroupFamilies::turnOnVariable(CoordinatesVariable * coordinates)
{
	int varFamIndex=coordinates->getVarFamIndex();
	int varIndex=coordinates->getVarIndex();
	
	VariableFamily * auxVarFam=families[varFamIndex];
	auxVarFam->turnOnVariable(varIndex);
}

void GroupFamilies::turnOffVariable(CoordinatesVariable * coordinates)
{
	int varFamIndex=coordinates->getVarFamIndex();
	int varIndex=coordinates->getVarIndex();
	
	VariableFamily * auxVarFam=families[varFamIndex];
	auxVarFam->turnOffVariable(varIndex);
}

void GroupFamilies::updateValues(vector<int> * variablesState)
{
	if(variablesState->size() != families.size())
	{
		throw ("Vectors must be from equal size");
	}

	int variableToTurnOn;
	for(int i=0;i<families.size();i++)
	{
		variableToTurnOn=(*variablesState)[i];
		if(variableToTurnOn<0)
		{
			families[i]->clear();
		}
		else
		{
			families[i]->turnOnVariable(variableToTurnOn);
		}
	}
}

void GroupFamilies::print()
{
    
    //el numero de actividad no corresponde
    for(int i=0; i<numberFamilies;i++){
        families[i]->print();
        
    }
    
}


 

vector<string> GroupFamilies::getArrRest()
{
    string aux;
    vector<string> restrictions;
    
    //el numero de actividad no corresponde
    //Al slot que va a tener en el vector
    for(int i=0; i<numberFamilies;i++){
        aux = families[i]->getRestriccion();
        restrictions.push_back(aux);
        
    }
    
    return restrictions;
    
    
    
}
