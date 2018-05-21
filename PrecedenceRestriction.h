/* 
 * File:   PrecedenceRestriction.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:06 AM
 */

#ifndef PRECEDENCERESTRICTION_H
#define PRECEDENCERESTRICTION_H

#include <iostream>
#include "VariableFamily.h"
#include "Graph.h"
#include "AllCandidateStations.h"
#include <vector>

using namespace std;

class PrecedenceRestriction
{
public:
	PrecedenceRestriction(VariableFamily * origin, VariableFamily * destination);
	bool isCorrect();
	void addConstantDestination(int a);
	void addConstantOrigin(int a);
	void print();
        int getOrigin();
        int getDestination();
        
        
        
private:
	VariableFamily * origin;
	VariableFamily * destination;
	int constant;
	int getValue();
	int getSumOrigin();
	int getSumDestination();
	void addToConstant(int a);
};

#endif