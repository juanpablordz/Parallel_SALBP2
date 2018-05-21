/* 
 * File:   Simplifier.h
 * Author: juanpablo
 *
 * Created on June 12, 2017, 12:08 PM
 */

#ifndef SIMPLIFIER_H
#define SIMPLIFIER_H

#include <iostream>
#include "Activity.h"
#include "Graph.h"
#include "AllCandidateStations.h"
#include "ActivityCandidateStations.h"
#include <vector>

using namespace std;

class Simplifier
{
public:
	Simplifier(Graph * graph, double heuristic, int numberStations);
	AllCandidateStations * simplify();
	void setHeuristic(double heuristic);

private:
	Graph * graph;
	double heuristic;
	int numberStations;
	
	// Functions to calculate forward and backward times from activities
	void calculateTimes();
	void calculateTimesForward();
	void calculateTimesBackward();
	double calculateTimeForward(Activity * act, vector<bool> * visited);
	double calculateTimeBackward(Activity * act, vector<bool> * visited);
	vector<bool> * getFalseActArray();

	// Functions to find the candidate stations for the activities
	AllCandidateStations * findCandidates();
	AllCandidateStations * getForwardCandidates();
	AllCandidateStations * getBackwardCandidates();
	void assignCandStationsFwd(ActivityCandidateStations * activityCand, int interval);
	void assignCandStationsBwd(ActivityCandidateStations * activityCand, int interval);
	int searchInterval(double time);
	vector<int> getIntervals();
};

#endif
