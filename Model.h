/* 
 * File:   Model.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:06 AM
 */

#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include "Equation.h"
#include "Restrictions.h"
#include <vector>

using namespace std;

class Model
{
public:
	Model(int numberEquations, vector<vector<int>> initialSolution);
	Equation * getEquation(int numEquation);
	bool isCorrect();
	void initializeRestrictions(GroupFamilies * groupFam , AllCandidateStations * candidates, Graph * graph);
	void print();
	int getNumberEquations();
	void updateObjective(double objective);
	double getMaxValue();
	void updateSolution();
	vector<vector <int>> getSolution();
        vector<PrecedenceRestriction *> getPrecedenceRestrictions();
        vector<string> getUnicityRestrictions();
        vector<Equation *> getVectorEquations();
        
        
private:
	vector<Equation *> equations;
	Restrictions * restrictions;
	int numberEquations;
	vector<vector <int>> solution;
};

#endif