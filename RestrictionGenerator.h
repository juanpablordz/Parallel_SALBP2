/* 
 * File:   RestrictionGenerator.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 1:14 PM
 */

#ifndef RESTRICTIONGENERATOR_H
#define	RESTRICTIONGENERATOR_H

#include <iostream>
#include "AllCandidateStations.h"
#include "GroupFamilies.h"
#include "Model.h"
#include "Graph.h"
#include "AssemblyLine.h"
#include "InfoVariableAssignation.h"
#include <vector>
#include <stack>
#include <bitset>

class RestrictionGenerator {
public:
	RestrictionGenerator(double minValue, double maxValue, AllCandidateStations * simplifiedModel, Graph * graph, AssemblyLine * assemblyLine);
	void print();
	void printSolution();
        
        Model * getModel();
        GroupFamilies * getUnicityRestrictions();
        
private:

    	Model * model;
	GroupFamilies * groupFam;
	Graph * graph;
	AssemblyLine * assemblyLine;
	double maxValue;
	double minValue;

	void createObjects(AllCandidateStations * simplifiedModel);
	vector<Constant *> initializeGroupFamAndVectorConstants(AllCandidateStations * simplifiedModel);
	void initializeModel(vector<Constant *> constants, AllCandidateStations * simplifiedModel);
	vector<Constant *> getVectorEmptyConstants(int numStations);
	vector<int> getCandidateStationNumbers();

	void updateValuesInfo(InfoVariableAssignation * info, CoordinatesVariable * coordinatesMaxFam, vector<double> * valuesEquations);
	bool updateMaxValue(double maxValue);
	void initializeVectorValuesEquations(vector<double> * valuesEquations);

	void printState(stack<int> familyExploration);
	void printVarState(vector<int> variablesState);
};

#endif	/* RESTRICTIONGENERATOR_H */

