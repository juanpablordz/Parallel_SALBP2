/* 
 * File:   Constant.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:05 AM
 */

#include "stdafx.h"
#include "Constant.h"

Constant::Constant(Station * assocStation)
{
	value=0;
	Constant::assocStation=assocStation;
}

double Constant::getValue()
{
	return value;
}

vector<Activity *> Constant::getAssocActivities()
{
	return assocActivities;
}

int Constant::getStationNumber()
{
	return assocStation->number;
}

void Constant::addActivity(Activity * act)
{
	assocActivities.push_back(act);
	value+=act->duration;
}