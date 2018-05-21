/* 
 * File:   Restrictions.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:06 AM
 */

#include <iostream>
#include "stdafx.h"
#include "Restrictions.h"

Restrictions::Restrictions(vector<Equation *> * equations, GroupFamilies * groupFam , AllCandidateStations * candidates, Graph * graph)
{
	Restrictions::equations=equations;
	Restrictions::groupFam=groupFam;
	initializePrecRestrictions(candidates, graph);
	minValue=-1;
}

void Restrictions::setMinValue(double minValue)
{
	Restrictions::minValue=minValue;
}

bool Restrictions::isCorrect()
{
	//if(! areBinaryMutuallyExclusiveRestrictionsCorrect())
	//{
	//	throw exception("Something went wrong assigning the variables a flag");
	//}

	if(!areMinValueRestrictionsCorrect())
	{
		return false;
	}

	if(!arePrecRestrictionsCorrect())
	{
		return false;
	}

	return true;
}

void Restrictions::initializePrecRestrictions(AllCandidateStations * candidates, Graph * graph)
{
	ActivityCandidateStations * auxCandOrigin;
	ActivityCandidateStations * auxCandDest;
	VariableFamily * auxVarFamOrigin;
	VariableFamily * auxVarFamDestination;
	PrecedenceRestriction * auxRestriction;

	int numberAct=candidates->getNumberActivities();
	for(int i=0;i<numberAct;i++)
	{
		for(int j=0;j<numberAct;j++)
		{
			if(graph->areAdjacent(i,j))
			{
				auxCandOrigin=candidates->getCandStationsForActivity(i);
				auxCandDest=candidates->getCandStationsForActivity(j);
				if(isPrecRestrictionActive(auxCandOrigin,auxCandDest))
				{
					auxVarFamOrigin=groupFam->getFamily(graph->getActivity(i));
					auxVarFamDestination=groupFam->getFamily(graph->getActivity(j));
					
					auxRestriction=new PrecedenceRestriction(auxVarFamOrigin,auxVarFamDestination);
					precRestrictions.push_back(auxRestriction);

					// If either of both are NULL means that it has a unique candidate!
					if(auxVarFamOrigin==NULL)
					{
						auxRestriction->addConstantOrigin(auxCandOrigin->getLastCandidateStationSet()+1);
					}
					if(auxVarFamDestination==NULL)
					{
						auxRestriction->addConstantDestination(auxCandDest->getLastCandidateStationSet()+1);
					}
				}
			}
		}
	}
}

//aqui puede estar el detalle

bool Restrictions::isPrecRestrictionActive(ActivityCandidateStations * originCand, ActivityCandidateStations * destinationCand)
{
	if(originCand->hasOnlyOneCandidate() && destinationCand->hasOnlyOneCandidate())
	{
		return false;
	}

	int numberStations=originCand->getNumberStations();
	bool stCandOrigin, stCandDestination;

	int i;
	for(i=0;i<numberStations;i++)
	{
		stCandDestination=destinationCand->isStationCandidate(i);
		if(stCandDestination)
		{
			break;
		}
	}

	int j;
	for(j=numberStations-1;j>=0;j--)
	{
		stCandOrigin=originCand->isStationCandidate(j);
		if(stCandOrigin)
		{
			break;
		}
	}

	if(i==numberStations || j<0)
	{
		throw ("Didn't find candidate for activity");
	}

	// We got to the end of the array of destination meaning Min(dest)=MAX ie no problem
	if(i==numberStations-1)
	{
		return false;
	}

	if(j<=i)
	{
		return false;
	}

	return true;
}

bool Restrictions::arePrecRestrictionsCorrect()
{
	int numberRestrictions=precRestrictions.size();
	PrecedenceRestriction * auxPrecRest;

	for(int i=0;i<numberRestrictions;i++)
	{
		auxPrecRest=precRestrictions[i];
		if(!auxPrecRest->isCorrect())
		{

			return false;
		}
	}
	return true;
}

bool Restrictions::areBinaryMutuallyExclusiveRestrictionsCorrect()
{
	int numberFamilies=groupFam->getNumberFamilies();
	VariableFamily * auxVarFam;

	for(int i=0;i<numberFamilies;i++)
	{
		auxVarFam=groupFam->getFamily(i);
		if(!auxVarFam->isCorrect())
		{
			return false;
		}
	}
	return true;
}

bool Restrictions::areMinValueRestrictionsCorrect()
{
	double valueEq;
	int numberEquations=equations->size();

	for(int i=0;i<numberEquations;i++)
	{
		valueEq= (*equations)[i]->getValue();
		if(valueEq>=minValue)
		{
			return false;
		}
	}
	return true;
}

void Restrictions::printPrecedence()
{
	for(int i=0;i<precRestrictions.size();i++)
	{
		precRestrictions[i]->print();
	}
}

vector<PrecedenceRestriction *> Restrictions::getPrecedenceRestrictions()
{
    return precRestrictions;
   
}

void Restrictions::printUnicity()
{
    groupFam->print();
            
}

 
 
vector<string> Restrictions::getUnicity()
{
  
    return groupFam->getArrRest();
            
}
