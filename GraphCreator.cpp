/* 
 * File:   GraphCreator.cpp
 * Author: juanpablo
 * 
 * Created on June 10, 2017, 2:13 AM
 */

#include "stdafx.h"
#include "GraphCreator.h"
#include "Utilities.h"


GraphCreator::GraphCreator()
{
	numberLinesRead=0;
	numberActivity=0;
        
}

Graph* GraphCreator::createGraph(string path)
{
	Graph * graph=new Graph();
        bool flag = true;
	fr.path=path;
	fr.openFile();
	// TODO: Repair isEOF
	//while(!fr.isEOF())
	while(!fr.infile.eof() && flag)
	{
		line=fr.getLine();
                cout << line << endl;
		flag = interpretLine(line, graph);
		numberLinesRead++;
	}

	fr.closeFile();

	// Gets are here to initialize origin and destination values
	graph->getOrigin();
	graph->getDestination();

	labelActivities(graph);
 

	return graph;
}


void GraphCreator::labelActivities(Graph * graph)
{
	Activity * aux;
	for(int i=0;i<graph->getNumberActivities();i++)
	{
		aux=graph->getActivity(i);
		aux->number=i;
	}
}

/*

Interpret Line
El objetivo de este metodo es ejecutar el metodo correspondiente a la line que leí.
1. Numero de Actividades
2. Duracion de actvidades (n lineas)
3. Precedencia entre actividades (n lineas)

*/

bool GraphCreator::interpretLine(string line, Graph * graph)
{
    bool flag = true;
	if(line.compare("")!=0){
		if(numberLinesRead==0)
		{
			readNumberOfActivities(line, graph);
		}
		else if(numberLinesRead <= graph->getNumberActivities())
		{
			readDurationActivity(line, graph, numberActivity);
			numberActivity++;
		}
		else
		{
                        flag = readRelation(line, graph);
		}
	}
    
    return flag;
}

void GraphCreator::readNumberOfActivities(string line, Graph * graph)
{
	int numberActivities;

	numberActivities=atoi(line.c_str());
	graph->setNumberActivities(numberActivities);
        //durationArr[numberActivities-1] = {};
        
	for(int i=0; i<numberActivities; i++)
	{
		graph->setActivity(i,new Activity());
	}
}

void GraphCreator::readDurationActivity(string line, Graph * graph, int numberActivity)
{
	Activity * activity;

	activity=graph->getActivity(numberActivity);
	double duration;
	duration=atof(line.c_str());
	activity->duration=duration;
        
	graph->addDurationToTotal(duration);
}


/*
Cada renglon tiene una rlacion de precedencia

origen, destino
*/

bool GraphCreator::readRelation(string line, Graph * graph)
{
	int origin;
	int dest;
        bool flag = true;
       
	fr.parseTwoValues(line, &origin, &dest);
        
	if(origin != -1 && dest != -1)
	{
                origin--;
                dest--;
                graph->setAdjacent(origin,dest);
	}
        else
        {
            flag = false;
        }    
        
        return flag;
        
}
 