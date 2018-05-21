/* 
 * File:   Model.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:06 AM
 */

#include <iostream>
#include "stdafx.h"
#include "Model.h"

Model::Model(int numberEquations, vector<vector<int>> initialSolution)
{
	solution=initialSolution;
	for(int i=0;i<numberEquations;i++)
	{
		equations.push_back(new Equation());
	}
	Model::numberEquations=numberEquations;
}

Equation * Model::getEquation(int numEquation)
{
	return equations[numEquation];
}



bool Model::isCorrect()
{
	return restrictions->isCorrect();
}

void Model::initializeRestrictions(GroupFamilies * groupFam , AllCandidateStations * candidates, Graph * graph)
{
	restrictions= new Restrictions(&equations,groupFam,candidates,graph);
}

void Model::print()
{
    cout << "-------- PRECEDENCE RESTRICTIONS --------" << endl;
    restrictions->printPrecedence();
    
    cout << endl;
    
    cout << "-------- DURATION RESTRICTIONS --------" << endl;
    for(int i=0; i<numberEquations; i++){
        equations[i]->print();
    }
    
    cout << endl;
    
    cout << "-------- UNICITY RESTRICTIONS --------" << endl;
    restrictions->printUnicity();
    
    cout << endl;
    
}


vector<PrecedenceRestriction *> Model::getPrecedenceRestrictions()
{

    return restrictions->getPrecedenceRestrictions();
   
}

//METODO QUE REGRESE EL ARREGLO 



int Model::getNumberEquations()
{
	return equations.size();
}

void Model::updateObjective(double objective)
{
	restrictions->setMinValue(objective);
}

double Model::getMaxValue()
{
	double max=-1;
	double aux;
	for(int i=0;i<equations.size();i++)
	{
		aux=equations[i]->getValue();
		if(max<aux)
		{
			max=aux;
		}
	}
	return max;
}

void Model::updateSolution()
{
	solution.clear();

	Equation * auxEq;
	for(int i=0;i<equations.size();i++)
	{
		auxEq=equations[i];
		solution.push_back(auxEq->getSetActivitiesIndexes());
	}
}

vector<vector <int>> Model::getSolution()
{
	return solution;
}

vector<Equation *> Model::getVectorEquations(){
    return equations;
    
}

vector<string> Model::getUnicityRestrictions()
{
	return restrictions->getUnicity();
}