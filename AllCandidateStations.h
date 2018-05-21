/* 
 * File:   AllCandidateStations.h
 * Author: juanpablo
 *
 * Created on June 12, 2017, 12:08 PM
 */

#ifndef ALLCANDIDATESTATIONS_H
#define	ALLCANDIDATESTATIONS_H

#include <iostream>
#include "ActivityCandidateStations.h"
#include "Graph.h"
#include <vector>
using namespace std;


class AllCandidateStations
{
public:
	AllCandidateStations(Graph * graph, int numberStations);
	int getNumberActivities();
	int getNumberStations();
	void addCandStationToActivity(int activityIndex, int stationIndex);
	ActivityCandidateStations * getCandStationsForActivity(int activityIndex);
	static AllCandidateStations * intersect(AllCandidateStations * a, AllCandidateStations * b);
	bool isFeasible();
	void print();
        int ** generateTable();

private:
	AllCandidateStations(vector<ActivityCandidateStations *> candStations, int numberActivities, int numberStations);

	vector<ActivityCandidateStations *> candStations;	
	int numberActivities;
	int numberStations;
        
        
        
        
};

#endif