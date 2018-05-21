/* 
 * File:   Activity.cpp
 * Author: juanpablo
 * 
 * Created on June 9, 2017, 9:51 PM
 */

#include <iostream>
#include "Activity.h"

Activity::Activity()
{
	isAssigned=false;
	timeForward=-1;
	timeBackward=-1;
}

Activity::Activity(double duration)
{
	Activity::duration=duration;
	isAssigned=false;
	timeForward=-1;
	timeBackward=-1;
}

void Activity::setTimeForward(double time)
{
	timeForward=time;
}

void Activity::setTimeBackward(double time)
{
	timeBackward=time;
}

double Activity::getTimeForward()
{
	return timeForward;
}

double Activity::getTimeBackward()
{
	return timeBackward;
}

// Operadores para realizar operaciones de comparacion

bool Activity::operator< (const Activity& other) const
{
	return duration < other.duration;
}

bool Activity::operator> (const Activity& other) const
{
	return duration > other.duration;
}

// Not intuitive... Left this way to provide consistency with
// the other two overridden operators
bool Activity::operator== (const Activity& other) const
{
	return duration == other.duration;
}

void Activity::printActivity()
{
	printf("Activity %3d-> Duration: %10.3f  ||  timeF: %10.3f  ||  timeB: %10.3f\n",number+1,duration,timeForward,timeBackward);
}