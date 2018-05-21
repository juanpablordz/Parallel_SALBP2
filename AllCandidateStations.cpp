/* 
 * File:   AllCandidateStations.cpp
 * Author: juanpablo
 * 
 * Created on June 12, 2017, 12:08 PM
 */

#include <iostream>
#include "stdafx.h"
#include "AllCandidateStations.h"
#include "ActivityCandidateStations.h"

AllCandidateStations::AllCandidateStations(vector<ActivityCandidateStations *> candStations, int numberActivities, int numberStations)
{
	AllCandidateStations::candStations=candStations;
	AllCandidateStations::numberActivities=numberActivities;
	AllCandidateStations::numberStations=numberStations;
           
}

AllCandidateStations::AllCandidateStations(Graph * graph, int numberStations)
{
	AllCandidateStations::numberActivities=graph->getNumberActivities();
	AllCandidateStations::numberStations=numberStations;
	for(int i=0;i<numberActivities;i++)
	{
		candStations.push_back(new ActivityCandidateStations(numberStations, graph->getActivity(i)));
	}
        
}

int AllCandidateStations::getNumberActivities()
{
	return numberActivities;
}

int AllCandidateStations::getNumberStations()
{
	return numberStations;
}

void AllCandidateStations::addCandStationToActivity(int activityIndex, int stationIndex)
{
	ActivityCandidateStations * actCandSt=candStations[activityIndex];
	actCandSt->setStationCandidate(stationIndex);
}

ActivityCandidateStations * AllCandidateStations::getCandStationsForActivity(int activityIndex)
{
	return candStations[activityIndex];
}

AllCandidateStations * AllCandidateStations::intersect(AllCandidateStations * a, AllCandidateStations * b)
{
	if(a->numberActivities!=b->numberActivities)
	{
		throw ("Arrays with different sizes");
	}

	ActivityCandidateStations * aux=NULL;
	vector<ActivityCandidateStations *> candSt;

	for(int i=0;i<a->numberActivities;i++)
	{
		aux=ActivityCandidateStations::intersect(a->getCandStationsForActivity(i), b->getCandStationsForActivity(i));
		candSt.push_back(aux);
	}

	return new AllCandidateStations(candSt,a->numberActivities,aux->getNumberStations());
}

bool AllCandidateStations::isFeasible()
{
	for(int i=0; i<candStations.size();i++)
	{
		if(candStations[i]->hasNoCandidate())
		{
			return false;
		}
	}
	return true;
}

void AllCandidateStations::print()
{
	for(int i=0;i<numberActivities;i++)
	{
		printf("Activity %d:\n",i);
		candStations[i]->print();
	}
	printf("\n");
}

//Tabla en la que como columnas tenemos a las distintas actividades de la red y como renglones tenemos las posibles
//estaciones. La casilla [estacion][actividad]=1 solamente cuando es posible que la actividad i se asignada
// a la estacion j.

int** AllCandidateStations::generateTable()
{
    int est = numberStations;
    int act = numberActivities;
    
    //Creacion de la matriz
    int **arr = new int*[est];
    for(int i = 0; i < est; ++i) {
        arr[i] = new int[act];
    }
   
    //Relleno de la matriz 
    for(int w=0; w<act;w++){
        for(int z=0; z<est; z++){
            arr [z][w] = candStations[w]->getCandidates()[z];
        } 
    }
       

    for(int j=0; j<est; j++){
        for(int k=0; k<act; k++){
            cout << arr [j][k];
        }
        cout << endl;
    }
    
    return arr;

    
    
    
    
}