/* 
 * File:   Heuristic.h
 * Author: juanpablo
 *
 * Created on June 12, 2017, 11:32 AM
 */

#ifndef HEURISTIC_H
#define	HEURISTIC_H

/*
#ifndef OPTIMIZER_H
#define OPTIMIZER_H
 */

#include "AssemblyLine.h"
#include <vector>
#include "Graph.h"

class Heuristic
{
public:
	Heuristic(Graph * graph, AssemblyLine * assemblyLine);
	void solve();
	double getHeuristic();

private:
	Graph * graphModel;
	AssemblyLine * assemblyLine;

	static const int kAssignedAct=1000;

	vector<bool> assignedActivities;
	int numberActivitiesAssigned;
	priority_queue<Activity, vector<Activity>,greater<Activity> > frontCandidateActs;
	priority_queue<Activity, vector<Activity>,greater<Activity> > backCandidateActs;
	Station * currFrontStation;
	Station * currBackStation;
	vector<int> unassignedActivites;
	double unassignedTime;
	double availableTimePerStation;
	int stationsOpen;
	double heuristicTime;

	Activity * resolveConflictBetweenActivities(Activity * activityA, Station * stationA,Activity * activityB, Station * stationB, Station ** stationToAssignTo);
	bool assignActivity(Activity * activity, Station * station, bool ignoreTimeRestraint);
	void calculateSumDuration();
	void calculateAvailableTimePerSt();
	bool shouldActivityBeAssigned(double delta, double time, int stationsOpen);
	Activity * getNextPriActivity(priority_queue<Activity, vector<Activity>,greater<Activity>> * priQueue);
	void removeTopActivity(priority_queue<Activity, vector<Activity>,greater<Activity>> * priQueue);
	Station * getNextStation(Station * st, bool fromFront);
	void activityWasAssigned(Activity * act);
	void assignTimeToStation(Station * st);
	void checkClosedStation();
};

#endif