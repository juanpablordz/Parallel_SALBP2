/* 
 * File:   Graph.h
 * Author: juanpablo
 *
 * Created on June 9, 2017, 9:01 PM
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <deque>
#include <vector>
#include "Activity.h"
using namespace std;

class Graph
{
public:
	Graph();
	Graph(int numberActivities);
	void setNumberActivities(int numberActivities);
	int getNumberActivities() const;
	Activity * getOrigin();
	Activity * getDestination();
	bool areAdjacent(Activity * source, Activity * dest);
	bool areAdjacent(int source, int dest);
	void setAdjacent(int source, int dest);
	void setActivity(int index, Activity * act);
	Activity * getActivity(int index);
	void addDurationToTotal(double duration);
	double getTotalDuration();
	void printGraph();
        void printActivityColletion();
        int ActivityDurationsGDC();
        int gcd(int a, int b);

	bool artificialOrigin;
	bool artificialDestination;

private:
	deque<deque<int>> adjMatrix;
	deque<Activity *>  activityCollection;
	Activity * kOrigin;
	Activity * kDestination;
	
	Activity * origin;
	Activity * destination;
	int numberActivities;

	double totalDuration;

        
        vector<int> durationArr;
        
	void initializeGraph(int numberActivities);
	void defaultValues();
	void createArtificialOrigin(deque<int> originList);
	void createArtificialDestination(deque<int> destinationList);
};

#endif
