/* 
 * File:   Heuristic.cpp
 * Author: juanpablo
 * 
 * Created on June 12, 2017, 11:32 AM
 */

#include <iostream>
#include "stdafx.h"
#include "Heuristic.h"
#include "Utilities.h"
using namespace std;

Heuristic::Heuristic(Graph * graph, AssemblyLine * assemblyLine)
{
	Heuristic::graphModel=graph;
	Heuristic::assemblyLine=assemblyLine;

	numberActivitiesAssigned=0;
	unassignedTime=0;
	availableTimePerStation=0;

	stationsOpen=assemblyLine->getNumberStations();
	int numberActivities= graph->getNumberActivities();

	calculateSumDuration();
	calculateAvailableTimePerSt();

	currFrontStation=assemblyLine->getStation(0);
	currBackStation=assemblyLine->getStation(stationsOpen-1);
	currFrontStation->isBeingUsed=true;
	currBackStation->isBeingUsed=true;
	assignTimeToStation(currFrontStation);
	assignTimeToStation(currBackStation);

	assignedActivities.resize(numberActivities);
	unassignedActivites.resize(numberActivities);

	for(int i=0;i<numberActivities;i++)
	{
		unassignedActivites[i]=0;
	}

	Activity * origin=graphModel->getOrigin();
	Activity * destination=graphModel->getDestination();

	unassignedActivites[origin->number]=kAssignedAct;
	unassignedActivites[destination->number]=kAssignedAct;


	frontCandidateActs.push(*origin);
	backCandidateActs.push(*destination);

	heuristicTime=0;
}

void Heuristic::calculateSumDuration()
{
	for(int i=0;i<graphModel->getNumberActivities();i++)
	{
		unassignedTime+= graphModel->getActivity(i)->duration;
	}
}

void Heuristic::calculateAvailableTimePerSt()
{
	availableTimePerStation=round(unassignedTime/stationsOpen,true);
}

// Function to be used just when we know 
Activity * Heuristic::resolveConflictBetweenActivities(Activity * activityA, Station * stationA,Activity * activityB, Station * stationB, Station ** stationToAssignTo)
{
	double timeA, timeB, availA, availB;
	timeA=activityA->duration;
	timeB=activityB->duration;
	availA=stationA->getAvailableTime();
	availB=stationB->getAvailableTime();

	double deltaA, deltaB;
	deltaA=availA-timeA;
	deltaB=availB-timeB;

	if(deltaA<0 && deltaB<0)
	{
		deltaA*=-1;
		deltaB*=-1;

		if(deltaA<=deltaB)
		{
			*stationToAssignTo=stationA;
			if(shouldActivityBeAssigned(deltaA,timeA,stationsOpen))
			{
				return activityA;
			}
		}
		else
		{
			*stationToAssignTo=stationB;
			if(shouldActivityBeAssigned(deltaB,timeB,stationsOpen))
			{
				return activityB;
			}
		}
		return NULL;
	}
	else
	{
		throw ("Error: Wrong usage");
	}
}

bool Heuristic::shouldActivityBeAssigned(double delta, double time, int stationsOpen)
{
	int T= round(time/(stationsOpen-1),true);
	return delta<=T;
}

bool Heuristic::assignActivity(Activity * activity, Station * station, bool ignoreTimeRestraint)
{
	if(!ignoreTimeRestraint)
	{
		double time, availableTime, delta;
		time=activity->duration;
		availableTime=station->getAvailableTime();
		delta=availableTime-time;

		if(delta>=0)
		{
			station->assignActivity(activity);
			activityWasAssigned(activity);
			numberActivitiesAssigned++;
			return true;
		}
		else
		{
			return false;
		}

	}
	else
	{
		station->assignActivity(activity);
		activityWasAssigned(activity);
		numberActivitiesAssigned++;
		return true;
	}
}


void Heuristic::assignTimeToStation(Station * st)
{
	st->setTotalTime(availableTimePerStation);
	unassignedTime-=availableTimePerStation;
}

void Heuristic::activityWasAssigned(Activity * act)
{
	act->isAssigned=true;
	assignedActivities[act->number]=true;
	//unassignedTime-=act->duration;
	unassignedActivites[act->number]=kAssignedAct;
	bool isUnblockedFront=true;
	bool isUnblockedBack=true;

	for(int i=0;i<unassignedActivites.size();i++)
	{
		if(i!=act->number && unassignedActivites[i]!=kAssignedAct)
		{
			for(int j=0;j<graphModel->getNumberActivities();j++)
			{
				// Check if blocked in front
				if(graphModel->areAdjacent(j,i))
				{
					isUnblockedFront= (isUnblockedFront && assignedActivities[j]);
				}

				// Check if blocked in back
				if(graphModel->areAdjacent(i,j))
				{
					isUnblockedBack= (isUnblockedBack && assignedActivities[j]);
				}
			}

			if(isUnblockedFront)
			{
				frontCandidateActs.push(*graphModel->getActivity(i));
			}

			if(isUnblockedBack)
			{
				backCandidateActs.push(*graphModel->getActivity(i));
			}
		}



		isUnblockedFront=true;
		isUnblockedBack=true;
	}
}

void Heuristic::solve()
{
	Activity * frontCandAct=NULL;
	Activity * backCandAct=NULL;
	bool unblockedFront, unblockedBack;
	unblockedBack=true;
	unblockedFront=true;

	bool recalcCandF=true;
	bool recalcCandB=true;
        
        
        
	while(true)
	{

		if(recalcCandF || frontCandAct->isAssigned)
		{
			frontCandAct=getNextPriActivity(&frontCandidateActs);
		}

		if(recalcCandB || backCandAct->isAssigned)
		{
			backCandAct=getNextPriActivity(&backCandidateActs);
		}

		if(unblockedBack && unblockedFront)
		{
			unblockedFront=assignActivity(frontCandAct, currFrontStation,false);
			unblockedBack=assignActivity(backCandAct, currBackStation,false);

			recalcCandF=unblockedFront;
			recalcCandB=unblockedBack;
		}
		else if(unblockedBack)
		{
			unblockedBack=assignActivity(backCandAct, currBackStation,false);

			recalcCandB=unblockedBack;
		}
		else if(unblockedFront)
		{
			unblockedFront=assignActivity(frontCandAct, currFrontStation,false);

			recalcCandF=unblockedFront;
		}
		else
		{
                        
			Activity * toAssign;
			Station * which=NULL;

			toAssign=resolveConflictBetweenActivities(frontCandAct,currFrontStation,backCandAct,currBackStation,&which);


			// toAssign is NULL only when the station should be closed

			// If the station should be closed, candidate activities must not
			// be recalculated. The station is now unblocked.

			// BEWARE: Begins short-circuiting!!
			// recalc's might change if toAssign == NULL
			if(which==currFrontStation)
			{
				unblockedFront=true;
				recalcCandF=true;
				recalcCandB=false;
			}
			else if(which==currBackStation)
			{
				unblockedBack=true;
				recalcCandF=false;
				recalcCandB=true;
			}
			else
			{
				throw ("Error: Pointers are different.");
			}
			// ENDS short-circuiting

			if(toAssign!=NULL)
			{
				assignActivity(toAssign, which,true);
			}
			else
			{
				which->closeStation();
				recalcCandF=false;
				recalcCandB=false;
			}
		}

		// We know we have finished when we have assigned all the activities
		if(numberActivitiesAssigned==assignedActivities.size() || stationsOpen==0)
		{
			break;
		}

		checkClosedStation();

		if(currBackStation==NULL)
		{
			unblockedBack=false;
			recalcCandB=false;
		}
	}
        

	heuristicTime= assemblyLine->getMaxTime();
}

double Heuristic::getHeuristic()
{
	if(heuristicTime==0)
	{
		solve();
	}
	return heuristicTime;
}

void Heuristic::checkClosedStation()
{
	bool isFrontStOpen=currFrontStation->getIsOpen();
	bool isBackStOpen= (currBackStation==NULL?false:currBackStation->getIsOpen()); // if back station is null, open is false
	if(!isFrontStOpen || !currBackStation->getIsOpen())
	{
		if(!isFrontStOpen && !isBackStOpen)
		{
			stationsOpen-=2;

			unassignedTime+=currFrontStation->getAvailableTime();
			unassignedTime+=currBackStation->getAvailableTime();
			unassignedTime+=(availableTimePerStation-currFrontStation->getTotalTime());
			unassignedTime+=(availableTimePerStation-currBackStation->getTotalTime());

			currFrontStation=getNextStation(currFrontStation, true);
			currBackStation=getNextStation(currBackStation, false);

		} 
		else if(!isFrontStOpen)
		{
			stationsOpen--;

			unassignedTime+=currFrontStation->getAvailableTime();
			unassignedTime+=currBackStation->getTotalTime();
			unassignedTime+=(availableTimePerStation-currFrontStation->getTotalTime());

			currFrontStation=getNextStation(currFrontStation,true);

		}
		else if(!isBackStOpen)
		{
			stationsOpen--;

			unassignedTime+=currBackStation->getAvailableTime();
			unassignedTime+=currFrontStation->getTotalTime();
			unassignedTime+=(availableTimePerStation-currBackStation->getTotalTime());

			currBackStation=getNextStation(currBackStation,false);
		}

		// Asignar tiempos
		if(stationsOpen==0)
		{
			return;
		}

		calculateAvailableTimePerSt();

		if(currFrontStation==NULL || currBackStation==NULL)
		{
			if(currFrontStation==NULL)
			{
				currFrontStation=currBackStation;
				assignTimeToStation(currFrontStation);
			}
		}
		else
		{
			assignTimeToStation(currFrontStation);
			assignTimeToStation(currBackStation);
		}


	}
}


Activity * Heuristic::getNextPriActivity(priority_queue<Activity, vector<Activity>,greater<Activity>> * priQueue)
{
	Activity auxQ=priQueue->top();
	priQueue->pop();
	int number=auxQ.number;
	Activity * auxG=graphModel->getActivity(number);
	while(!priQueue->empty() && auxG->isAssigned)
	{
		auxQ=priQueue->top();
		priQueue->pop();
		number=auxQ.number;
		auxG=graphModel->getActivity(number);
	}

	if(priQueue->empty()==true && auxG->isAssigned)
	{
		return NULL;
	}


	return auxG;
}

void Heuristic::removeTopActivity(priority_queue<Activity, vector<Activity>,greater<Activity>> * priQueue)
{
	priQueue->pop();
}

Station * Heuristic::getNextStation(Station * st, bool fromFront)
{
	Station * next;
	if(fromFront)
	{
		next=st->next;
	}
	else
	{
		next=st->prev;
	}

	if(next->isBeingUsed)
	{
		return NULL;
	}

	next->isBeingUsed=true;

	return next;
}

