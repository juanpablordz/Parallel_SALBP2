/* 
 * File:   Simplifier.cpp
 * Author: juanpablo
 * 
 * Created on June 12, 2017, 12:08 PM
 */

#include <iostream>
#include "stdafx.h"
#include "Simplifier.h"

Simplifier::Simplifier(Graph * graph, double heuristic, int numberStations)
{
	Simplifier::graph=graph;
	Simplifier::heuristic=heuristic;
	Simplifier::numberStations=numberStations;
}

void Simplifier::setHeuristic(double heuristic)
{
	Simplifier::heuristic=heuristic;
}

void Simplifier::calculateTimes()
{
	calculateTimesForward();
	calculateTimesBackward();
}

void Simplifier::calculateTimesForward()
{
	Activity * act;
	double time;
	for(int i=0; i<graph->getNumberActivities();i++)
	{
		act=graph->getActivity(i);
		if(act->getTimeForward()<0)
		{
			time=calculateTimeForward(act, getFalseActArray());
			act->setTimeForward(time);
		}
	}
}

void Simplifier::calculateTimesBackward()
{
	Activity * act;
	double time;
	for(int i=0; i<graph->getNumberActivities();i++)
	{
		act=graph->getActivity(i);
		if(act->getTimeBackward()<0)
		{
			time=calculateTimeBackward(act, getFalseActArray());
			act->setTimeBackward(time);
		}
	}
}


double Simplifier::calculateTimeForward(Activity * act, vector<bool> * visited)
{
	double time=0;

	(*visited)[act->number]=true;

	for(int i=0; i<graph->getNumberActivities();i++)
	{
		if(!(*visited)[i] && graph->areAdjacent(act->number,i))
		{
			(*visited)[i]=true;
			time+=calculateTimeForward(graph->getActivity(i),visited);

		}
	}
	time+=act->duration;
	return time;
}

double Simplifier::calculateTimeBackward(Activity * act, vector<bool> * visited)
{
	double time=0;

	(*visited)[act->number]=true;

	for(int i=0; i<graph->getNumberActivities();i++)
	{
		if(!(*visited)[i] && graph->areAdjacent(i,act->number))
		{
			(*visited)[i]=true;
			time+=calculateTimeBackward(graph->getActivity(i),visited);

		}
	}
	time+=act->duration;
	return time;
}

vector<bool> * Simplifier::getFalseActArray()
{
	return new vector<bool>(graph->getNumberActivities(),false);
}
// si la interseccion me da vacia entonces aumento el ciclo en 1 iteracion
AllCandidateStations * Simplifier::findCandidates()
{
	AllCandidateStations * fwdCand=getForwardCandidates();
	AllCandidateStations * bwdCand=getBackwardCandidates();
	AllCandidateStations * intersection=AllCandidateStations::intersect(fwdCand,bwdCand);

	int numberAct=graph->getNumberActivities();
	ActivityCandidateStations * actCand;
	// Origin candidate is set to first station
	actCand= intersection->getCandStationsForActivity(0);
	actCand->assignUniqueCandidate(0);
	// Destination candidate is set to last station
	actCand= intersection->getCandStationsForActivity(numberAct-1);
	actCand->assignUniqueCandidate(numberStations-1);

	return intersection;
}

int Simplifier::searchInterval(double time)
{
	vector<int> intervals=getIntervals();

	for(int j=0;j<numberStations;j++)
	{
		if(time<=intervals[j])
		{
			return j;
		}
	}
	throw ("No interval found");
}

AllCandidateStations * Simplifier::getForwardCandidates()
{
	Activity * act;
	double time;
	int interval;
	AllCandidateStations * candidates=new AllCandidateStations(graph,numberStations);

	for(int i=0; i<graph->getNumberActivities();i++)
	{
		act=graph->getActivity(i);
		time=act->getTimeForward();
		interval=searchInterval(time);
		assignCandStationsFwd(candidates->getCandStationsForActivity(i),interval);
	}
	return candidates;
}

void Simplifier::assignCandStationsFwd(ActivityCandidateStations * activityCand, int interval)
{
	for(int i=0;i<numberStations-interval;i++)
	{
		activityCand->setStationCandidate(i);
	}
}

AllCandidateStations * Simplifier::getBackwardCandidates()
{
	Activity * act;
	double time;
	int interval;
	AllCandidateStations * candidates=new AllCandidateStations(graph,numberStations);

	for(int i=0; i<graph->getNumberActivities();i++)
	{
		act=graph->getActivity(i);
		time=act->getTimeBackward();
		interval=searchInterval(time);
		assignCandStationsBwd(candidates->getCandStationsForActivity(i),interval);
	}
	return candidates;
}

void Simplifier::assignCandStationsBwd(ActivityCandidateStations * activityCand, int interval)
{
	for(int i=interval;i<numberStations;i++)
	{
		activityCand->setStationCandidate(i);
	}
}
//POR AQUI
vector<int> Simplifier::getIntervals()
{
	vector<int> intervals;
	for(int i=1; i<numberStations+1; i++) // "1 based" to perform multiplication of H
	{
		intervals.push_back(i*heuristic);
	}
	return intervals;
}

AllCandidateStations * Simplifier::simplify()
{
	calculateTimes();
	return findCandidates();
}