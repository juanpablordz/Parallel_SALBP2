/* 
 * File:   Activity.h
 * Author: juanpablo
 *
 * Created on June 9, 2017, 9:51 PM
 */

#ifndef ACTIVITY_H
#define ACTIVITY_H

class Activity
{
public:
	int number;
	double duration;
	bool isAssigned;
	double timeForward; // ->
	double timeBackward; // <-

	Activity();
	Activity(double duration);

	void setTimeForward(double time);
	void setTimeBackward(double time);
	double getTimeForward();
	double getTimeBackward();
	void printActivity();

	bool operator< (const Activity& other) const;
	bool operator> (const Activity& other) const;
	bool operator== (const Activity& other) const;
};

#endif