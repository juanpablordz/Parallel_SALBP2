/* 
 * File:   Station.h
 * Author: juanpablo
 *
 * Created on June 10, 2017, 1:58 AM
 */
#ifndef STATION_H
#define STATION_H

#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include "Activity.h"
using namespace std;

class Station
{

public:
	Station();

	int number;
	bool isBeingUsed;
	Station * next;
	Station * prev;

	int getNumberOfStation();
	void setTotalTime(double totalTime);
	bool getIsOpen();
	double getAvailableTime();
	double getTotalTime();
	void assignActivity(Activity * activity);
	void printStation();
	void closeStation();
	vector<int> getAssignedActIndexes();
private:
	vector<Activity *> assignedActivities;
	
	double timeSpent;
	double totalTime;
	double availableTime;
	bool isOpen;
	

	
};


#endif