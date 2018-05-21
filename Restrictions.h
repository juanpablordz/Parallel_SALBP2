/* 
 * File:   Restrictions.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:06 AM
 */

#ifndef RESTRICTIONS_H
#define RESTRICTIONS_H

#include <iostream>
#include "Equation.h"
#include "PrecedenceRestriction.h"
#include "GroupFamilies.h"
#include "AllCandidateStations.h"
#include "Graph.h"
#include <vector>

using namespace std;

class Restrictions
{
public:
	Restrictions(vector<Equation *> * equations, GroupFamilies * groupFam , AllCandidateStations * candidates, Graph * graph);
	void setMinValue(double minValue);
	bool isCorrect();
	void printPrecedence();
        void printUnicity();
        vector<PrecedenceRestriction *> getPrecedenceRestrictions();
        vector<string> getUnicity();
        

private:
	vector<Equation *> * equations;
	vector<PrecedenceRestriction *> precRestrictions;
	GroupFamilies * groupFam;
	double minValue;

	void initializePrecRestrictions(AllCandidateStations * candidates, Graph * graph);
	bool isPrecRestrictionActive(ActivityCandidateStations * originCand, ActivityCandidateStations * destinationCand);

	bool arePrecRestrictionsCorrect();
	bool areBinaryMutuallyExclusiveRestrictionsCorrect();
	bool areMinValueRestrictionsCorrect();

};

#endif