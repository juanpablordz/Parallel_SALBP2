/* 
 * File:   Balas.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:07 AM
 */

#ifndef BALAS_H
#define BALAS_H

#define MAX_NUM_VARS 80

#include <iostream>
//#include <boost>
#include "AllCandidateStations.h"
#include "GroupFamilies.h"
#include "Model.h"
#include "Graph.h"
#include "AssemblyLine.h"
#include "InfoVariableAssignation.h"
#include <vector>
//#include "../boost/dynamic_bitset.hpp"
//#include "boost/dynamic_bitset.hpp"
#include <stack>
#include <bitset>

#define PRODUCTION
//#define BENCHMARK

using namespace std;


class Balas
{
public:
	Balas(double minValue, double maxValue, AllCandidateStations * simplifiedModel, Graph * graph, AssemblyLine * assemblyLine);
	double solveModel();
	void print();
	void printSolution();
	
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

	void exploreTreeItMem();
	void searchFromAllFamiliesMem(vector<bool> * usedFamilies, InfoVariableAssignation * info, vector<double> valuesEquations);
	void searchFromVariablesWithinFamilyMem(int familyIndex, bitset<MAX_NUM_VARS> *  usedVariables, InfoVariableAssignation * info, vector<double> valuesEquations);
	void initializeVectorValuesEquations(vector<double> * valuesEquations);

	void printState(stack<int> familyExploration);
	void printVarState(vector<int> variablesState);
	
};

#endif