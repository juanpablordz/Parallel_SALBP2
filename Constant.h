/* 
 * File:   Constant.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:05 AM
 */

#ifndef CONSTANT_H
#define CONSTANT_H

#include <iostream>
#include "Activity.h"
#include "Station.h"
#include <vector>

using namespace std;

class Constant
{
public:
	Constant(Station * assocStation);
	double getValue();
	vector<Activity *> getAssocActivities();
	int getStationNumber();
	void addActivity(Activity * act);
private:
	double value;
	vector<Activity *> assocActivities;
	Station * assocStation;
};

#endif