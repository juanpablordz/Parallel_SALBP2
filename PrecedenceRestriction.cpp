/* 
 * File:   PrecedenceRestriction.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:06 AM
 */

#include <iostream>
#include "stdafx.h"
#include "PrecedenceRestriction.h"


PrecedenceRestriction::PrecedenceRestriction(VariableFamily * origin, VariableFamily * destination)
{
	PrecedenceRestriction::origin=origin;
	PrecedenceRestriction::destination=destination;
	constant=0;
}

int PrecedenceRestriction::getOrigin()
{
    return origin->getAssocActivityNumber();
}

int PrecedenceRestriction::getDestination()
{
    return destination->getAssocActivityNumber();
}

bool PrecedenceRestriction::isCorrect()
{
	if(origin->hasActiveVariable() && !destination->hasActiveVariable())
	{
		int origActive=origin->getActiveVariable();
		int destMax=destination->getNumberVariables()-1;
		Variable * varOrig=origin->getVariable(origActive);
		Variable * varDest=destination->getVariable(destMax);
		
		return varDest->getStationNumber() >= varOrig->getStationNumber();
	}
	if(origin->hasActiveVariable() && destination->hasActiveVariable())
	{
		return getValue()>=0;
	}
	return true;
}

void PrecedenceRestriction::addToConstant(int a)
{
	constant+=a;
}

void PrecedenceRestriction::addConstantDestination(int a)
{
	if(a<0)
	{
		throw ("Number must be greater or equal to zero");
	}

	addToConstant(a);
}

void PrecedenceRestriction::addConstantOrigin(int a)
{
	if(a<0)
	{
		throw ("Number must be greater or equal to zero");
	}

	addToConstant(-a);
}

int PrecedenceRestriction::getValue()
{
	int difference;
	difference=getSumDestination()-getSumOrigin() + constant;
	return difference;
}

int PrecedenceRestriction::getSumOrigin()
{
	if(origin==NULL)
	{
		return 0;
	}

	int sum=0;

	int activeVar=origin->getActiveVariable();
	Variable * auxVar= origin->getVariable(activeVar);
	sum+=auxVar->getStationNumber()+1; // SUM{i*Ai}; Ai={0,1} for all i, i = {1, 2, ..., numberStations} (ie One based)
	return sum;
}

int PrecedenceRestriction::getSumDestination()
{
	if(destination==NULL)
	{
		return 0;
	}

	int sum=0;

	int activeVar=destination->getActiveVariable();
	Variable * auxVar= destination->getVariable(activeVar);
	sum+=auxVar->getStationNumber()+1; // SUM{i*Ai}; Ai={0,1} for all i, i = {1, 2, ..., numberStations} (ie One based)
	return sum;
}

 

void PrecedenceRestriction::print()
{
	int destActNumber=-1;
	int origActNumber=-1;

	if(origin!=NULL)
	{
		origActNumber=origin->getAssocActivityNumber();
	}
	if(destination!=NULL)
	{
		destActNumber=destination->getAssocActivityNumber();
	}

	printf("Origin Activity %d - Destination Activity %d\n", origActNumber, destActNumber);
        
}


