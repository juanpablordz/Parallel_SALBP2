/* 
 * File:   CoordinatesVariable.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:07 AM
 */

#include <iostream>
#include "stdafx.h"
#include "CoordinatesVariable.h"

CoordinatesVariable::CoordinatesVariable()
{
	varFamIndex=-1;
	varIndex=-1;
}

void CoordinatesVariable::updateVarFamIndex(int varFamIndex)
{
	CoordinatesVariable::varFamIndex=varFamIndex;
}

void CoordinatesVariable::updateVarIndex(int varIndex)
{
	CoordinatesVariable::varIndex=varIndex;
}

int CoordinatesVariable::getVarFamIndex()
{
	return varFamIndex;
}

int CoordinatesVariable::getVarIndex()
{
	return varIndex;
}