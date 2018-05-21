/* 
 * File:   AssemblyLine.cpp
 * Author: juanpablo
 * 
 * Created on June 10, 2017, 2:12 AM
 */

#include "stdafx.h"
#include "AssemblyLine.h"

AssemblyLine::AssemblyLine(int numberStations)
{
	AssemblyLine::numberStations=numberStations;
	//stations.resize(numberStations);
	Station * prev= new Station();
	prev->prev=NULL;
	prev->number=0;
	stations.push_back(prev);

	Station * next;
	for(int i=1; i<numberStations;i++)
	{
		next=new Station();
		next->number=i;
		prev->next=next;
		next->prev=prev;
		stations.push_back(next);
		prev=next;
	}

	maxTimeStation=0;
}

int AssemblyLine::getNumberStations()
{
	return numberStations;
}

Station * AssemblyLine::getStation(int i)
{
	return stations.at(i);
}

double AssemblyLine::getMaxTime()
{
	if(maxTimeStation==0)
	{
		double max=stations[0]->getTotalTime();
		for(int i=1;i<numberStations;i++)
		{
			if(stations[i]->getTotalTime()>max)
			{
				max=stations[i]->getTotalTime();
			}
		}
		maxTimeStation=max;
		
	}
	return maxTimeStation;
}


void AssemblyLine::printAssemblyLine()
{
	printf("==============  ASSEMBLY LINE  ==============\n");
	for(int i=0;i<numberStations;i++)
	{
		stations[i]->printStation();
	}
	printf("=============================================\n");
}

vector<vector<int>> AssemblyLine::getAssignationIndexes()
{
	vector<vector<int>> indexes;
	for(int i=0;i<stations.size();i++)
	{
		indexes.push_back(stations[i]->getAssignedActIndexes());
	}
	return indexes;
}