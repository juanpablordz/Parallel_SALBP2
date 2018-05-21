/* 
 * File:   ActivityCandidateStations.cpp
 * Author: juanpablo
 * 
 * Created on June 12, 2017, 12:07 PM
 */

#include <iostream>
#include <string>
#include "stdafx.h"
#include "ActivityCandidateStations.h"


//ActivityCandidateStations::ActivityCandidateStations()
//{
//	isUnique=false;
//	isEmpty=true;
//	numberCandidates=0;
//	lastCandSet=-1;
//}

ActivityCandidateStations::ActivityCandidateStations(int numberStations, Activity * assocActivity)
{
	candStations= vector<bool>(numberStations,false);
	ActivityCandidateStations::numberStations=numberStations;
	isUnique=false;
	isEmpty=true;
	numberCandidates=0;
	lastCandSet=-1;
	ActivityCandidateStations::assocActivity=assocActivity;
}

void ActivityCandidateStations::setStationCandidate(int stationIndex)
{
	isEmpty=false;
	numberCandidates++;
	isUnique= (numberCandidates==1);
	candStations[stationIndex]=true;
	lastCandSet=stationIndex;
}

bool ActivityCandidateStations::isStationCandidate(int stationIndex)
{
	return candStations[stationIndex];
}

int ActivityCandidateStations::getNumberStations()
{
	return numberStations;
}

ActivityCandidateStations * ActivityCandidateStations::intersect(ActivityCandidateStations * a, ActivityCandidateStations * b)
{
	if(a->numberStations != b->numberStations)
	{
		throw ("Arrays with different sizes");
	}
	if(a->assocActivity->number != b->assocActivity->number)
	{
		throw ("Intersection different activities");
	}

	ActivityCandidateStations * intersection= new ActivityCandidateStations(a->numberStations, a->assocActivity);

	for(int i=0; i<a->numberStations;i++)
	{
		if(a->isStationCandidate(i) && b->isStationCandidate(i))
		{
			intersection->setStationCandidate(i);
		}
	}

	return intersection;
}

bool ActivityCandidateStations::hasOnlyOneCandidate()
{
	return isUnique;
}

int ActivityCandidateStations::getLastCandidateStationSet()
{
	return lastCandSet;
}

Activity * ActivityCandidateStations::getAssocActivity()
{
	return assocActivity;
}

bool ActivityCandidateStations::hasNoCandidate()
{
	return isEmpty;
}

void ActivityCandidateStations::assignUniqueCandidate(int numberStCandidate)
{
	candStations= vector<bool>(numberStations,false);
	candStations[numberStCandidate]=true;
	isUnique=true;
	isEmpty=false;
	numberCandidates=1;
	lastCandSet=numberStCandidate;

}

void ActivityCandidateStations::print()
{
	for(int i=0;i<numberStations;i++)
	{
		if(candStations[i])
		{
			printf("%d ",i);
		}
	}
	printf("\n");
}

vector<bool> ActivityCandidateStations::getCandidates()
{
    return candStations;
    
}

string ActivityCandidateStations::getCandidatesExtra()
{
    string aux;
    
    	for(int i=0;i<numberStations;i++)
	{
		if(candStations[i])
		{
                    aux.append(to_string((long long)i));
		}
	}
    
    
    return aux;
    
}