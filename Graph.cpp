/* 
 * File:   Graph.cpp
 * Author: juanpablo
 * 
 * Created on June 9, 2017, 9:01 PM
 */



#include "Graph.h"

Graph::Graph()
{
	defaultValues();
	totalDuration=0;
}

// cuando cree uno nodo artificial tengo que modificar este tamaño de matriz 
//si es que se usa en el futuro para algun proceso

void Graph::initializeGraph(int numberActivities)
{
    
	Graph::numberActivities=numberActivities;
	activityCollection.resize(numberActivities);
	//Se inicializan "las columnas"
	adjMatrix.resize(numberActivities);
        //Se inicializan "los renglones"
	for(int i=0;(i<numberActivities);i++)
	{
		adjMatrix[i].resize(numberActivities);
	}
        
	defaultValues();
}

void Graph::defaultValues()
{
	kOrigin=new Activity(-1000);
	kDestination=new Activity(-1000);
	origin=kOrigin;
	destination=kDestination;
	artificialOrigin=false;
	artificialDestination=false;
}

Graph::Graph(int numberActivities)
{
	initializeGraph(numberActivities);
}

void Graph::setNumberActivities(int numberActivities)
{
	initializeGraph(numberActivities);
}

int Graph::getNumberActivities() const
{
	return adjMatrix.size();
}

Activity * Graph::getOrigin()
{
	if(origin==kOrigin)
	{
		deque<int> originList;
		int hits=0;

		// If zero column then origin
		for(int j=0;j<numberActivities;j++)
		{
			for(int i=0;i<numberActivities;i++)
			{
				hits+=adjMatrix[i][j];
			}
			if(hits<=0)
			{
				originList.push_back(j);
			}
			hits=0;
		}

		if(originList.size()==1)
		{
			origin=getActivity(originList.front());
		}
		else if(originList.size()>1)
		{
			//throw exception("Error: More than one possible origin");
			createArtificialOrigin(originList);
			origin=getActivity(0);
		}
		else
		{
			throw ("Error: No possible origin found");
		}

	}

	return origin;

}

void Graph::createArtificialOrigin(deque<int> originList)
{
	// Creation of artificial origin Activity
	artificialOrigin=true;
	numberActivities++;
	adjMatrix.push_front(deque<int> (numberActivities));
	for(int k=1;k<numberActivities;k++)
	{
		adjMatrix[k].push_front(0);
	}

	activityCollection.push_front(new Activity(0));


	// Creation of edges from origin towards Activities
	int dest;
	int numOrig=originList.size();
	for(int k=0; k<numOrig;k++)
	{
		dest=originList.back() + 1;
		adjMatrix[0][dest]=1;
		originList.pop_back();
	}

}

Activity * Graph::getDestination()
{
	if(destination==kDestination)
	{
		deque<int> destinationList;
		int hits=0;
		int i=0;
		int j=0;

		// If zero column then destination
		for(i=0;i<numberActivities;i++)
		{
			for(j=0;j<numberActivities;j++)
			{
				hits+=adjMatrix[i][j];
			}
			if(hits<=0)
			{
				destinationList.push_back(i);
			}
			hits=0;
		}

		
		if(destinationList.size()==1)
		{
			destination=getActivity( destinationList.front());
		}
		// If more than one destination we have to create an artificial
		// destination
		else if(destinationList.size()>1)
		{
			//throw exception("Error: More than one destination found");
			createArtificialDestination(destinationList);
			destination=getActivity( numberActivities-1);
		}
		else
		{
			throw ("Error: No possible destination found");
		}

	}

	return destination;
}

void Graph::createArtificialDestination(deque<int> destinationList)
{
	artificialDestination=true;
	for(int i=0;i<numberActivities;i++)
	{
		adjMatrix[i].push_back(0);
	}

	numberActivities++;
	adjMatrix.push_back(deque<int> (numberActivities));

	activityCollection.push_back(new Activity(0));

	int orig;
	int numDest=destinationList.size();
	for(int i=0;i<numDest;i++)
	{
		orig=destinationList.back();
		adjMatrix[orig][numberActivities-1]=1;
		destinationList.pop_back();
	}
}

bool Graph::areAdjacent(Activity * source, Activity * dest)
{
	return areAdjacent(source->number,dest->number);
}

bool Graph::areAdjacent(int source, int dest)
{
	int cell=adjMatrix[source][dest];
	return cell==1;
}

void Graph::setAdjacent(int source, int dest)
{
	adjMatrix[source][dest]=1;

}

void Graph::setActivity(int index, Activity * act)
{
	activityCollection[index]=act;
}

Activity * Graph::getActivity(int index)
{
	return activityCollection[index];
}

void Graph::addDurationToTotal(double duration)
{
    durationArr.push_back(duration);
	totalDuration+=duration;
}

double Graph::getTotalDuration()
{
	return totalDuration;
}

void Graph::printGraph()
{
    
    for(int k = 0 ; k<numberActivities; k++){
        cout << "\t" << k;       
    }
    printf("\n");
	for(int i=0;i<numberActivities;i++)
	{
            cout << i << "\t";
		for(int j=0;j<numberActivities;j++)
		{
			cout << adjMatrix[i][j] << "\t";
		}
		printf("\n");
	}
}

void Graph::printActivityColletion()
{
    for(int i=0; i< activityCollection.size(); i++){
        activityCollection[i]->printActivity();
        
    }
    
}

int Graph::ActivityDurationsGDC()
{
    int result = durationArr[0];
    for (int i=1; i<durationArr.size()-1; i++)
		result = gcd(durationArr[i], result);

	return result;
    
}

// Function to return gcd of a and b
int Graph::gcd(int a, int b)
{
	if (a == 0)
		return b;
	return gcd(b%a, a);
}


