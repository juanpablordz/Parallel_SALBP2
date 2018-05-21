/* 
 * File:   ActivityCandidateStations.h
 * Author: juanpablo
 *
 * Created on June 12, 2017, 12:07 PM
 */

#ifndef ACTIVITYCANDIDATESTATIONS_H
#define ACTIVITYCANDIDATESTATIONS_H

#include <iostream>
#include "Activity.h"
#include <vector>
using namespace std;

class ActivityCandidateStations
{
public:
	ActivityCandidateStations(int numberStations, Activity * assocActivity);
	void setStationCandidate(int stationIndex);
	bool isStationCandidate(int stationIndex);
	int getNumberStations();
	static ActivityCandidateStations * intersect(ActivityCandidateStations * a, ActivityCandidateStations * b);
	void print();
	bool hasOnlyOneCandidate();
	int getLastCandidateStationSet();
	bool hasNoCandidate();
	Activity * getAssocActivity();
	void assignUniqueCandidate(int numberStCandidate);
        vector<bool> getCandidates();
        string getCandidatesExtra();
        
private:
	vector<bool> candStations;
	int numberStations;
	bool isUnique;
	bool isEmpty;
	int numberCandidates;
	int lastCandSet;
	Activity * assocActivity;
};

#endif