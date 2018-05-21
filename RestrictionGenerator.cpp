/* 
 * File:   RestrictionGenerator.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 1:14 PM
 */

#include "RestrictionGenerator.h"
#include "Model.h"

//constructor
RestrictionGenerator::RestrictionGenerator(double minValue, double maxValue, AllCandidateStations * simplifiedModel, Graph * graph, AssemblyLine * assemblyLine) {
    	RestrictionGenerator::minValue=minValue;
	RestrictionGenerator::maxValue=maxValue;
	RestrictionGenerator::graph=graph;
	RestrictionGenerator::assemblyLine=assemblyLine;
	createObjects(simplifiedModel);
}

void RestrictionGenerator::createObjects(AllCandidateStations * simplifiedModel)
{
	vector<Constant *> constants = initializeGroupFamAndVectorConstants(simplifiedModel);
	initializeModel(constants, simplifiedModel);
}

vector<Constant *> RestrictionGenerator::initializeGroupFamAndVectorConstants(AllCandidateStations * simplifiedModel)
{
	GroupFamilies * groupFamilies= new GroupFamilies();
	int numStations=simplifiedModel->getNumberStations();

	vector<Constant *> constants=getVectorEmptyConstants(numStations);

	int numActivites=simplifiedModel->getNumberActivities();
	ActivityCandidateStations * actCandSt;

	int stationNumber;
	Activity * auxActivity;
	Variable * auxVar;
	VariableFamily * auxVarFam;
	int familyIndex=0;

	for(int i=0;i<numActivites;i++)
	{
		actCandSt=simplifiedModel->getCandStationsForActivity(i);
		// If it only has one candidate then it is a constant
		if(actCandSt->hasOnlyOneCandidate())
		{
			stationNumber=actCandSt->getLastCandidateStationSet();
			auxActivity=actCandSt->getAssocActivity();
			constants[stationNumber]->addActivity(auxActivity);
		}
		// If has more than one candidate, then it is a variable family
		else
		{
			auxVarFam=new VariableFamily(familyIndex);
			familyIndex++;
			for(int j=0;j<actCandSt->getNumberStations();j++)
			{
				if(actCandSt->isStationCandidate(j))
				{
					auxVar=new Variable(graph->getActivity(i),assemblyLine->getStation(j));
					auxVarFam->insertVariable(auxVar);
				}
			}
			groupFamilies->insertFamily(auxVarFam);
		}
	}

	RestrictionGenerator::groupFam=groupFamilies;
	return constants;
}

vector<Constant *> RestrictionGenerator::getVectorEmptyConstants(int numStations)
{
	vector<Constant *> constants;
	for(int i=0;i<numStations;i++)
	{
		constants.push_back(new Constant(assemblyLine->getStation(i)));
	}
	return constants;
}

void RestrictionGenerator::initializeModel(vector<Constant *> constants, AllCandidateStations * simplifiedModel)
{
	int numEquations= constants.size();
	model= new Model(numEquations, assemblyLine->getAssignationIndexes());

	Equation * auxEq;

	for(int i=0; i<numEquations;i++)
	{
		auxEq=model->getEquation(i);
		auxEq->setConstant(constants[i]);
		auxEq->setAssocStation(assemblyLine->getStation(i));
	}

	int numberFamilies= groupFam->getNumberFamilies();
	VariableFamily * auxVarFam;
	Variable * auxVar;
	int auxStationNumber;
	int numberVariables;

	for(int i=0;i<numberFamilies;i++)
	{
		auxVarFam=groupFam->getFamily(i);
		numberVariables=auxVarFam->getNumberVariables();
		for(int j=0;j<numberVariables;j++)
		{
			auxVar=auxVarFam->getVariable(j);
			auxStationNumber=auxVar->getStationNumber();
			auxEq=model->getEquation(auxStationNumber);
			auxEq->insertVariable(auxVar);
		}
	}
	model->initializeRestrictions(groupFam,simplifiedModel,graph);
	model->updateObjective(maxValue);
        
       
}

Model * RestrictionGenerator::getModel(){
    
    return model;
    
}





