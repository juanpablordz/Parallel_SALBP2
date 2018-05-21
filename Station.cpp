/* 
 * File:   Station.cpp
 * Author: juanpablo
 * 
 * Created on June 10, 2017, 1:58 AM
 */

#include <iostream>
#include "Station.h"

Station::Station()
{
	isOpen=true;
	isBeingUsed=false;
	timeSpent=0;
	totalTime=0;
	availableTime=0;
}

void Station::setTotalTime(double totalTime)
{
	if(totalTime<=Station::timeSpent)
	{
		//throw exception("Error: Assigned time is less than spent time.");
		//Station::isOpen=false;
	}
	Station::totalTime=totalTime;
	Station::availableTime=totalTime-timeSpent;
}


int Station::getNumberOfStation()
{
	return number;
}

bool Station::getIsOpen()
{
	return isOpen;
}

double Station::getAvailableTime()
{
	return availableTime;
}

double Station::getTotalTime()
{
	return totalTime;
}

void Station::assignActivity(Activity * activity)
{
	assignedActivities.push_back(activity);
	timeSpent+=activity->duration;
	if(totalTime<timeSpent)
	{
		totalTime=timeSpent;
		availableTime=0;

	}
	else
	{
		availableTime=totalTime-timeSpent;
	}

	if(availableTime==0)
	{
		isOpen=false;
	}
}

void Station::printStation()
{
	printf("Station %3d [%.2f]:\n",number, timeSpent);
	for(int i=0;i<assignedActivities.size();i++)
	{
		printf("\t");
		assignedActivities[i]->printActivity();
	}
	printf("\n");
}

void Station::closeStation()
{
	isOpen=false;
}

vector<int> Station::getAssignedActIndexes()
{
	vector<int> indexes;
	for(int i=0;i<assignedActivities.size();i++)
	{
		indexes.push_back(assignedActivities[i]->number);
	}
	sort(indexes.begin(),indexes.end());
	return indexes;
}