/* 
 * File:   Variable.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:06 AM
 */

#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include "Activity.h"
#include "Station.h"
#include "Variable.h"

class Variable
{
public:
	Variable(Activity * assocActivity, Station * assocStation);
        Variable();
	void setFlag(bool value);
	bool isOn();
	double getValue();
	double getDuration();
	int getActivityNumber();
	int getStationNumber();
        void print();
        string getActSta();
private:
	int flag;
	Activity * assocActivity;
	Station * assocStation;
};

#endif