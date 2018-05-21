/* 
 * File:   InfoVariableAssignation.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:07 AM
 */

#include <iostream>
#include "stdafx.h"
#include "InfoVariableAssignation.h"

InfoVariableAssignation::InfoVariableAssignation()
{
	//maxValue=MAX_DEFAULT;
	maxVarIndex=-1;
	maxVarFamIndex=-1;
	
}

void InfoVariableAssignation::updateValues(vector<double> * values, CoordinatesVariable * coordinates)
{
	updateMax(values, coordinates);
	return;

}


CoordinatesVariable * InfoVariableAssignation::getCoordinatesAssignationMaxMax()
{
	CoordinatesVariable * maxC= new CoordinatesVariable();
	maxC->updateVarIndex(maxVarIndex);
	maxC->updateVarFamIndex(maxVarFamIndex);
	return maxC;

}


void InfoVariableAssignation::flush()
{
	//maxValue=MAX_DEFAULT;
	maxVarIndex=-1;
	maxVarFamIndex=-1;
	
	//return;

	//for(int i=0;i<infoEquations.size();i++)
	//{
	//	infoEquations[i]->flush();
	//}
}

void InfoVariableAssignation::updateMax(vector<double> * values, CoordinatesVariable * coordinates)
{
	int numberValues=values->size();

	for(int i=0;i<numberValues;i++)
	{
		if((*values)[i]>maxValue)
		{
			maxValue=(*values)[i];
			maxVarIndex=coordinates->getVarIndex();
			maxVarFamIndex=coordinates->getVarFamIndex();
		}
	}
}