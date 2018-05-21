/* 
 * File:   main.cpp
 * Author: juanpablo
 *
 * Created on June 7, 2017, 7:22 PM
// */
//

#include <string>
#include "Activity.h"
#include "Graph.h"
#include "Station.h"

#include "GraphCreator.h"
#include "FileReader.h"
#include "AssemblyLine.h"
#include "Utilities.h"
#include "Heuristic.h"
#include "Simplifier.h"
#include "RestrictionGenerator.h"
#include "Model.h"
#include "DurationNode.h"
#include "DurationTree.h"
//#include "strtk.hpp"
#include "Mixer.h"
#include "BinaryRestriction.h"
#include "VectorBR.h"
#include "BinaryMixer.h"
#include <cmath>
#include <chrono>
#include "stdafx.h"
#include <iostream>

#include <cuda.h>
#include <cuda_runtime.h>


/////////////////////////////////////////
// declaration, forward
/////////////////////////////////////////
//extern "C" int h_cont;

extern "C" bool 
runTest(int argc, const char **argv, unsigned long long int * h_cont, int numStations, unsigned int * h_temp, unsigned long long int  h_temp_size, unsigned int * h_rest, int h_rest_size, unsigned int * h_out);

using namespace std;
//using namespace std::chrono;

bool exist_test (const std::string& name){
    ifstream f(name.c_str());
    return f.good();
}

unsigned int hashmap_hash_int(unsigned long long int map_size, unsigned int key) {
	/* Robert Jenkins' 32 bit Mix Function */
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);

	/* Knuth's Multiplicative Method */
	key = (key >> 3) * 2654435761;

	return key % map_size;
}

unsigned long long int MurmurHash2_pt1(unsigned long long int key, unsigned long long int seed)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	key *= m;
	key ^= key >> r;
	key *= m;

	seed *= m;
	seed ^= key;

	return seed;
}

unsigned long long int MurmurHash2_pt2(unsigned long long int seed)
{
	const unsigned int m = 0x5bd1e995;
	seed ^= seed >> 13;
	seed *= m;
	seed ^= seed >> 15;

	return seed;
}


unsigned long long int fnv1(unsigned long long int key, unsigned long long int res) {
	const unsigned long long int prime = 16777619;
	res = res ^ key;
	res = res * prime;
	return res;
}

//    heuristic = ceil(heuristic);

bool execute(){
    
    //Read Input File
	string filename = "C:\\Users\\FERNYBOY\\Desktop\\Data_BL\\JAESCHKE.IN2";
    bool exist = exist_test(filename);
    
    //Create Information Graph
    GraphCreator gc;
    Graph * graph = gc.createGraph(filename);

    int gdc = graph->ActivityDurationsGDC();
    //graph->printGraph();
    //graph->printActivityColletion();
    //cout << graph->getTotalDuration() << endl;
    
    // Determine number of Stations for the desired BALANCING.
    // User input.
   
    int numberStations = 4;
    
    
    // Create Assembly Line Model
    // Solve Heuristic for this number of stations. 
    // CHANGE: I have to change this so the program start on the first
    // possible balancing and then iterate in increments of the minimum common denominator of the stations duration.
    
    AssemblyLine * al = new AssemblyLine(numberStations);
    Heuristic * op=new Heuristic(graph,al);
    op->solve();
    
    //verificar que es lo que hace este metodo es necesario? solve for heuristic
    //Determine balancing duration to begin cycle.


	double heuristic = op->getHeuristic();
	cout << "Heuristic Balancing Time: " << heuristic << endl;

	//Change the value of heuristic for the ideal balancing time. The program iterates upwards in variations of the size of the 'Max common divisor'
	heuristic = graph->getTotalDuration() / numberStations;
	heuristic = ceil(heuristic);
	cout << "Ideal Balancing Time: " << heuristic << endl;
    


    
    
    //While loop that will be used to increment the balacing time of the iteration in 
    //intervals equal to the minimum common divisor of all activity durations.
    bool finish = false;
    while(!finish){
        
        cout << "--------------------------------------------------------------------------------------------------" << endl;
        cout << "\n \n"<< endl;
        cout << "Tiempo de Balanceo: " << heuristic << endl;
        cout << "\n \n"<< endl;
        cout << "--------------------------------------------------------------------------------------------------" << endl;


        // Create Simplifier for model
        Simplifier s(graph,heuristic,numberStations);
        AllCandidateStations * cand= s.simplify();
        
        // Binary Search for a lower limit
        // Determine which is the objective of this segment of code that simplifies the model
    
   
        double lowerLimit, upperLimit, middleValue;
        lowerLimit= graph->getTotalDuration()/numberStations;
        lowerLimit= round(lowerLimit,true);
        upperLimit=heuristic;

        double minValue,maxValue;

        while(lowerLimit<=upperLimit)
        {
                middleValue = round((lowerLimit+upperLimit)/2,true);
                s.setHeuristic(middleValue);
                cand = s.simplify();

                if(cand->isFeasible())
                {
                        upperLimit=middleValue-1;
                }
                else
                {
                        lowerLimit=middleValue+1;
                }

                delete cand;
        }

        cand = s.simplify();
        // If it is feasible return this one
        if(cand->isFeasible())
        {
                lowerLimit=middleValue;
        }
        // If it is not, return next (+1)
        else
        {
                lowerLimit=middleValue+1;
        }

        minValue=lowerLimit;
        maxValue=heuristic;

        // Step 3: Finished getting lower bound


        s.setHeuristic(heuristic);
        cand= s.simplify();

        // Step 4: Finished simplifying model
        
        
        // Constraint Generation
        RestrictionGenerator * r = new RestrictionGenerator(minValue,maxValue,cand,graph,al);
        
        //Nose que hace aqui
        Model * model= r->getModel();
        
        //Table of candidate assignation to stations universe
        int** tablaUniverso = cand->generateTable();
        
        // "precedenceR" es un arreglo de objetos (PrecedenceRestrictions) que vamos a usar para estraer las ecuaciones booleanas
        // de las restricciones que si son validas, es decir que generan una restriccion.
        vector<PrecedenceRestriction *> arrPrecedence = model->getPrecedenceRestrictions();

        // Generar restricciones de precedencia negadas
            /*
             * 1. Un metodo que extraiga de la restriccion la combiancion de letras que no se puede
             * 2. Determinar la negacion de esta expression
             * 3. Crear un arreglo de string
             * 4. Guardar las restricciones generadas en este arreglo de strings
             */

        /*
         * La idea va a ser recorrer la tabla del universo de variables en la columna de origen hacia abajo y para cada uno de los renglones
         * Verificar que no exista un valor possible anterior en estacion para el actual.
         * 

         */
        
            // ------RESTRICCIONES PRECEDENCIA BOOLEANAS----- 
    
            vector<string> rPrecedence;
            int origin, staOrigin;
            int dest, staDestination;
            int j = numberStations;
            bool flag = false;

            for(int i=0; i<arrPrecedence.size();i++){
                origin = arrPrecedence.at(i)->getOrigin();
                dest = arrPrecedence.at(i)->getDestination();
                while(!flag && j>0){
                    //[estacion][actividad]
                    if(tablaUniverso[j][origin]==1){
                        staOrigin=j;
                        flag = true;
                    }
                    j--;
                }

                flag=false;
                j=0;

                while(!flag && j<numberStations){
                    //[estacion][actividad]
                    if(tablaUniverso[j][dest]==1){
                        staDestination=j;
                        flag = true;
                    }
                    j++;
                }

                string temp; 
                string temp1 = getNegation(origin, staOrigin,tablaUniverso, numberStations);
                string temp2 = getNegation(dest, staDestination,tablaUniverso, numberStations);

                temp += temp1;
                temp += ",";
                temp += temp2;

               //cout << temp << endl;

                // agregar a arreglo de restricciones de Precedencia
                rPrecedence.push_back(temp);

            }
            
            // ------RESTRICCIONES UNICITY BOOLEANAS----- 

            vector<string> rUnicity = model->getUnicityRestrictions();

            string toString;
            int stationUnicity;
            for(int ii=0; ii< cand->getNumberActivities(); ii++){
                if(cand->getCandStationsForActivity(ii)->hasOnlyOneCandidate()){
                        string UnicityConstr;
                        toString = invHashAux(ii);
                        UnicityConstr.append(toString);
                        UnicityConstr.append(cand->getCandStationsForActivity(ii)->getCandidatesExtra());
                        rUnicity.push_back(UnicityConstr);

                }
            }
            
            //------RESTRICTIONS DURACION BOOLEANAS-----
            int numRecDuration = model->getNumberEquations();

            vector<Equation *> equations = model->getVectorEquations();

            vector<vector<DurationNode>> groupDN;

            for(int w=0; w<numRecDuration ; w++){

                vector<DurationNode> arr;
                int y= 0;
                Variable * aux;
                int balanceDuration = heuristic - equations[w]->getConstantValue();

                while(y<equations[w]->getNumVariables()){

                    aux = equations[w]->getVariable(y);
                    DurationNode * DN = new DurationNode( aux->getDuration(),aux->getActSta(), balanceDuration);  
                    //Add Duration Node to Vector
                    arr.push_back(*DN);
                    y = y+1;
                }

                //Add Vector of Duration Nodes to array of restrictions.
                groupDN.push_back(arr);
            }


            // Verificar la creacion de los nodos de duracion.
                
            int size = groupDN.size();
            vector<DurationNode> arr;

            for(int i =0; i<size ; i++){       
                arr = groupDN[i];
                groupDN[i] = MergeSort(arr);
                int numDNs= groupDN[i].size();
                for(int j =0; j<numDNs ; j++){
                    groupDN[i][j].setId(j);
                }
            }

            vector<string> vectorRest ;
            vector<string> * vectorR = &vectorRest;


            for(int j =0; j< groupDN.size(); j++){
                vector<DurationNode> * vectorDN = &groupDN[j];
                DurationTree * tree = new DurationTree(vectorDN, vectorR, groupDN[j][0].getBalanceDuration());
                tree->buildTree();

            }


            // "Negacion de Restricciones Generadas

            for(int i = 0; i < vectorR->size() ; i++){
                string negC = constraintNeg(vectorR->at(i), tablaUniverso, numberStations);
                vectorR->at(i) = negC;
            }
            
            //Licuadora
            Mixer * mixer = new Mixer();
            string resultadoMixer = vectorR->at(0);
            string tempM;

            for(int i = 1; i < vectorR->size(); i++){
                tempM = mixer->mix(resultadoMixer,vectorR->at(i));
                resultadoMixer = tempM;
            }

            for(int i = 0; i < rPrecedence.size() ; i++){
                tempM = mixer->mix(resultadoMixer,rPrecedence.at(i));
                resultadoMixer = tempM;
            }




            for(int i = 0; i < rUnicity.size() ; i++){
                tempM = mixer->mix(resultadoMixer,rUnicity.at(i));
                resultadoMixer = tempM;
            }

            cout << "Solucion: " << resultadoMixer << endl;

            if(resultadoMixer.size()!=0){
                finish = true;
            }
            
            // Aumentar iteracion de ciclo 

            heuristic = heuristic + gdc;


    }
    
    
    
    return true;

}
//
//int main(int argc, char** argv) {
//
//bool printExec(){
//      
//    cout << "TEST" << endl;
//    
//    cout << "READING FILE..." << endl;
// 
//    //Verify file existence
//    
//    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/BOWMAN8.IN2";
//    //Estaciones a verificar: 3 y 5
//    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JACKSON.IN2";
//    //Estaciones a verificar: 3,4 y 5
//    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JAESCHKE.IN2";
//    //Estaciones a verificar: 3,4,5 y 6 
//    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MANSOOR.IN2";
//    //Estaciones a verificar: 3 y 4
//    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MERTENS.IN2";
//    //Estaciones a verificar: 3 y 5
//    string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/LASCURAIN.IN2";
//    
//    
//    
//    bool exist = exist_test(filename);
//    
//    cout << "File existence: " << exist << endl;
//    
//
//    //Create Tables
//    //1. ForwardTimes, BackwardTimes
//    //2. Precedence Table
//
//    GraphCreator gc;
//
//    Graph * graph = gc.createGraph(filename);
//    graph->printGraph();
//    cout << "--------------------------------------------------------------------------------------------------" << endl;      
//    cout << "\n" << "Artificial Destination: " << graph->artificialDestination << endl;
//    cout << "Artificial Origin: " << graph->artificialOrigin << "\n " << endl;
//    
//    cout << "--------------------------------------------------------------------------------------------------" << endl;
//    cout << "Max common divisor: ";
//    int gdc = graph->ActivityDurationsGDC();
//    cout << gdc <<  endl;
//    
//    cout << "--------------------------------------------------------------------------------------------------" << endl;
//    cout << "Graph Total Duration: " << graph->getTotalDuration() << endl;
//    
//    
//    // Determine number of Stations for the desired BALANCING.
//    // User input.
//   
//    int numberStations = 3;
//    
//    
//    // Create Assembly Line Model
//    // Solve Heuristic for this number of stations. 
//    // CHANGE: I have to change this so the program start on the first
//    // possible balancing and then iterate in increments of the minimum common denominator of the stations duration.
//    
//    
//    AssemblyLine * al = new AssemblyLine(numberStations);
//    Heuristic * op=new Heuristic(graph,al);
//    op->solve();
//    double heuristic=op->getHeuristic();
//    cout << "Heuristic Balancing Time: " << heuristic << endl;
//    
//    //Change the value of heuristic for the ideal balancing time. The program iterates upwards in variations of the size of the 'Max common divisor'
//    heuristic = graph->getTotalDuration()/numberStations;
//    heuristic = ceil(heuristic);
//    cout << "Ideal Balancing Time: " << heuristic << endl;
//    
//    //heuristic = 2872;
//    
//    //Program starts here...
//    
//    //Flag for Balancing iteration loop
//    bool finish = false;
//    
//    //Balancing Time iteration loop
//    while(!finish){
//        
//    cout << "--------------------------------------------------------------------------------------------------" << endl;
//    cout << "\n\n"<< endl;
//    cout << "Tiempo de Balanceo: " << heuristic << endl;
//    cout << "\n\n"<< endl;
//    cout << "--------------------------------------------------------------------------------------------------" << endl;
//    
//    
//    // Create Simplifier for model
//    
//    Simplifier s(graph,heuristic,numberStations);
//    
//
//    
//    //esta bandera me va a ayudar a determinar si hay alguna interseccion vacia
//    // al momento de asignar actividades a estaciones.
//    // significa que no es possible este balanceo y tengo que iterar
//
//    AllCandidateStations * cand= s.simplify();
//    
//    if(cand->isFeasible())
//    {
//          
//    // Binary Search for a lower limit
//    // Determine which is the objective of this segment of code that simplifies the model
//    
//    double lowerLimit, upperLimit, middleValue;
//    lowerLimit= graph->getTotalDuration()/numberStations;
//    lowerLimit= round(lowerLimit,true);
//    upperLimit=heuristic;
//
//    double minValue,maxValue;
//
//    while(lowerLimit<=upperLimit)
//    {
//            middleValue = round((lowerLimit+upperLimit)/2,true);
//            s.setHeuristic(middleValue);
//            cand = s.simplify();
//
//            if(cand->isFeasible())
//            {
//                    upperLimit=middleValue-1;
//            }
//            else
//            {
//                    lowerLimit=middleValue+1;
//            }
//
//            delete cand;
//    }
//
//    cand = s.simplify();
//    // If it is feasible return this one
//    if(cand->isFeasible())
//    {
//            lowerLimit=middleValue;
//    }
//    // If it is not, return next (+1)
//    else
//    {
//            lowerLimit=middleValue+1;
//    }
//
//    minValue=lowerLimit;
//    maxValue=heuristic;
//
//    // Step 3: Finished getting lower bound
//
//
//    s.setHeuristic(heuristic);
//    cand= s.simplify();
//
//    // Step 4: Finished simplifying model
//		
//    cout << "Activity Information Table" << endl;
//    graph->printActivityColletion();
//    
//    cout << "--------------------------------------------------------------------------------------------------" << endl;
//    cout << "Candidate stations for each activity: " << endl;
//    cand->print();
//    
//    //Generate Model and Restrictions for Model
//    RestrictionGenerator * r = new RestrictionGenerator(minValue,maxValue,cand,graph,al);
//    Model * model= r->getModel();
//    
//    cout << "--------------------------------------------------------------------------------------------------" << endl;
//    cout << "------RESTRICCIONES----- \n\n" << endl;
//    
//    model->print();
//    
//    cout << "\n ------TABLA UNIVERSO----- \n" << endl;
//    int** tablaUniverso = cand->generateTable();
//    
//    // "precedenceR" es un arreglo de objetos (PrecedenceRestrictions) que vamos a usar para estraer las ecuaciones booleanas
//    // de las restricciones que si son validas, es decir que generan una restriccion.
//    vector<PrecedenceRestriction *> arrPrecedence = model->getPrecedenceRestrictions();
//    
//    // Generar restricciones de precedencia negadas
//        /*
//         * 1. Un metodo que extraiga de la restriccion la combiancion de letras que no se puede
//         * 2. Determinar la negacion de esta expression
//         * 3. Crear un arreglo de string
//         * 4. Guardar las restricciones generadas en este arreglo de strings
//         */
//    
//    /*
//     * La idea va a ser recorrer la tabla del universo de variables en la columna de origen hacia abajo y para cada uno de los renglones
//     * Verificar que no exista un valor possible anterior en estacion para el actual.
//     * 
//     
//     */
//    
//     cout << "\n\n ------RESTRICCIONES PRECEDENCIA BOOLEANAS----- \n\n" << endl;
//    
//    //formato correcto
//    vector<string> rPrecedence;
//    
//    
//    int origin, staOrigin;
//    
//    int dest, staDestination;
//    int j = numberStations-1;
//    int jDest = 0;
//    bool flag = false;
//    
//    for(int i=0; i<arrPrecedence.size();i++){
//        
//        origin = arrPrecedence.at(i)->getOrigin();
//        dest = arrPrecedence.at(i)->getDestination();
//        
//        cout << "ERROR EN PRECEDENCIA" << endl;
//        cout << "Origen: " << origin << endl;
//        cout << "Destino: " << dest << endl;
//        
//        while(j>0){
//            //[estacion][actividad]
//            if(tablaUniverso[j][origin]==1){
//                staOrigin=j;
//                flag = true;
//            }
//            
//            
//            while(flag && jDest<numberStations){
//            //[estacion][actividad]
//            if(tablaUniverso[jDest][dest]==1){
//                if(j>jDest){
//                    cout << "j: " << j << endl;
//                    cout << "JDest: " << jDest << endl;
//                    
//                    staDestination=jDest;
//                    string temp = ""; 
//                    string temp1 = getNegation(origin, staOrigin,tablaUniverso, numberStations);
//                    cout << "NEGACION ORIGIN: " << temp1 << endl;
//                    string temp2 = getNegation(dest, staDestination,tablaUniverso, numberStations);
//                    cout << "NEGACION DEST: " << temp2 <<endl;
//                    
//                    temp += temp1;
//                    temp += ",";
//                    temp += temp2;
//
//                    cout << temp << endl;
//
//                    // agregar a arreglo de restricciones de Precedencia
//                    rPrecedence.push_back(temp);
//
//                }
//            }
//            jDest++;
//            }
//            jDest = 0;
//            j--;
//            flag=false;
//
//        }
//        j = numberStations-1;
//        
//    }
//    
//    cout << "\n\n ------RESTRICTIONS UNICITY BOOLEANAS----- \n\n" << endl;
//    
//    
//    
//    
//    vector<string> rUnicity = model->getUnicityRestrictions();
//    
//    string toString;
//    int stationUnicity;
//    for(int ii=0; ii< cand->getNumberActivities(); ii++){
//        if(cand->getCandStationsForActivity(ii)->hasOnlyOneCandidate()){
//                string UnicityConstr;
//                toString = invHashAux(ii);
//                UnicityConstr.append(toString);
//                UnicityConstr.append(cand->getCandStationsForActivity(ii)->getCandidatesExtra());
//                rUnicity.push_back(UnicityConstr);
//            
//        }
//    }
//    
//    
//    
//
//
//    
//    
//    for(int i = 0; i< rUnicity.size() ;i++){
//            cout << rUnicity.at(i) << endl;
//    }
//    
//    //cand tiene el numero de actividades de ahi puedo generar la ultima restriccion
//    // number of stations numberStations
//    
//
//    
//            
//    
//    
//    cout << "\n\n ------RESTRICTIONS DURACION BOOLEANAS----- \n" << endl;
//    
//    int numRecDuration = model->getNumberEquations();
//    
//    
//
//    vector<Equation *> equations = model->getVectorEquations();
//
//    vector<vector<DurationNode>> groupDN;
//    
//    for(int w=0; w<numRecDuration ; w++){
//        
//        vector<DurationNode> arr;
//        int y= 0;
//        Variable * aux;
//        int balanceDuration = heuristic - equations[w]->getConstantValue();
//
//        while(y<equations[w]->getNumVariables()){
//            
//            aux = equations[w]->getVariable(y);
//            DurationNode * DN = new DurationNode( aux->getDuration(),aux->getActSta(), balanceDuration);  
//            //Add Duration Node to Vector
//            arr.push_back(*DN);
//            y = y+1;
//        }
//        
//        //Add Vector of Duration Nodes to array of restrictions.
//        groupDN.push_back(arr);
//    }
//    
//   
//    // Verificar la creacion de los nodos de duracion.
//    
//    int size = groupDN.size();
//
//    vector<DurationNode> arr;
//    
//
//    for(int i =0; i<size ; i++){      
//        
//        
//        arr = groupDN[i];
//        
//        //esta vacio
//        if(arr.size() != 0){
//            groupDN[i] = MergeSort(arr);
//
//            int numDNs= groupDN[i].size();
//            //cout << "\n \n";
//
//            for(int j =0; j<numDNs ; j++){
//                groupDN[i][j].setId(j);
//
//                // Imprimir los nodos ordenados.
//                /*
//                cout << "Nodo " << j << endl;
//                cout<< "Duracion: " << groupDN[i][j].getDuration() << endl;
//                cout<< "Restriction: " << groupDN[i][j].getRestriction() << endl;
//                cout<< "Id: " << groupDN[i][j].getId() <<endl;
//                cout<< "BalanceDuration: " << groupDN[i][j].getBalanceDuration() <<endl;
//                cout << "\n";
//                */
//            }
//            
//        }
//
//        
//        
//        
//    }
//
//    
//    vector<string> vectorRest ;
//    vector<string> * vectorR = &vectorRest;
//    
//    cout << groupDN.size() << endl;
//    for(int j =0; j< groupDN.size(); j++){
//        
//        vector<DurationNode> * vectorDN = &groupDN[j];
//        if(vectorDN->size() != 0){
//                DurationTree * tree = new DurationTree(vectorDN, vectorR, groupDN[j][0].getBalanceDuration());
//                tree->buildTree();
//        }
//
//        
//    }
//    
//    
//    
//    cout << "Restricciones Generadas:" << vectorR->size() << endl;
//    
//    
//    for(int i = 0; i < vectorR->size() ; i++){
//        cout << vectorR->at(i) << endl;
//    }
//    
//    
//    
//    
//
//
//    
//    
//        /*
//    cout << "Prueba de negacion de una restriccion: " << endl;
//    
//    string negC = constraintNeg("a0", tablaUniverso, numberStations);
//    
//    cout << "Negacion: " << negC << endl;
//    
//    cout << cand->getCandStationsForActivity(9)->getCandidatesExtra() << endl;
//    //cand->getCandStationsForActivity(0)->hasOnlyOneCandidate()
//         * */
//    
//    
//    cout << "Negacion de Restricciones Generadas:" << vectorR->size() << endl;
//    
//    for(int i = 0; i < vectorR->size() ; i++){
//        string negC = constraintNeg(vectorR->at(i), tablaUniverso, numberStations);
//        vectorR->at(i) = negC;
//        cout << vectorR->at(i) << endl;
//    }
//    
//    
//    cout << "Integracion Booleana" << endl;
//    
//    
//    
//    
//    
//    BinaryMixer bMixer;
//  
//    Mixer * mixer = new Mixer();
//    
//    //Primera restriccion entra al mixer. El mixer es donde se van a realizar las operaciones tipo AND.
//    string resultadoMixer = vectorR->at(0);
//    
//    VectorBR auxRtoB(numberStations, vectorR->at(0));
//    vector<BinaryRestriction> temp = auxRtoB.Br;
//    
//    cout << "size" << endl;
//    cout << temp.size() << endl;
//    
//    
//    cout << "\n Primera Restriccion: " << resultadoMixer << endl;
//    
//    
//    string tempM;
//    int contador= 0;
//    int totalRestrictions = vectorR->size() + rPrecedence.size() + rUnicity.size();
//
//    cout << "\n\n ------RESTRICCIONES PRECEDENCIA BOOLEANAS----- \n\n" << endl;
//    vector<VectorBR> rbPrecedence;
//    
//    for(int i = 0; i < rPrecedence.size() ; i++){
//        //convertir las restricciones a conjunte de restricciones binarias
//        VectorBR auxRtoB(numberStations, rPrecedence.at(i));
//        rbPrecedence.push_back(auxRtoB);
//        temp = bMixer.mix(temp,rbPrecedence.at(i).Br);
//   
//        
//        contador++;
//        cout << "Restriccion: " << rPrecedence.at(i) << endl;
//        tempM = mixer->mix(resultadoMixer,rPrecedence.at(i));
//        resultadoMixer = tempM;
//        cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
//        cout << resultadoMixer.size() << endl;
//        
//
//
//    }
//    
//    //imprimir numero de restricciones
//    cout << "Numero de restricciones de Precedencia: " << rPrecedence.size() << endl;
//    cout << "Numero de restricciones Binarias de Precedencia: " << rbPrecedence.size() << endl;
//    
//    
//
//    //Binarias
//    vector<VectorBR> rbDuracion;
//    
//    for(int i = 1; i < vectorR->size(); i++){
//        //convertir las restricciones a conjunte de restricciones binarias
//        VectorBR auxRtoB(numberStations, vectorR->at(i));
//        rbDuracion.push_back(auxRtoB);
//        temp = bMixer.mix(temp,rbDuracion.at(i-1).Br);
//        
//        contador++;
//        cout << "Restriccion: " << vectorR->at(i) << endl;
//        tempM = mixer->mix(resultadoMixer,vectorR->at(i));
//        resultadoMixer = tempM;
//        cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
//        cout << resultadoMixer.size() << endl;
//        
//
//    }
//    
//        //imprimir numero de restricciones
//    cout << "Numero de restricciones de Duracion: " << vectorR->size() << endl;
//    cout << "Numero de restricciones Binarias de Duracion: " << rbDuracion.size() << endl;
//    
//
//
//
//    //Binarias
//    vector<VectorBR> rbUnicity;
//    
//
//    for(int i = 0; i < rUnicity.size() ; i++){
//        //convertir las restricciones a conjunte de restricciones binarias
//        VectorBR auxRtoB(numberStations, rUnicity.at(i));
//        rbUnicity.push_back(auxRtoB);
//        temp = bMixer.mix(temp,rbUnicity.at(i).Br);
//        
//        contador++;
//        cout << "Restriccion: " << rUnicity.at(i) << endl;
//        tempM = mixer->mix(resultadoMixer,rUnicity.at(i));
//        resultadoMixer = tempM;
//        cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
//        cout << resultadoMixer.size() << endl;
//        //cout << resultadoMixer << endl;
//
//    }
//    //imprimir numero de restricciones
//    cout << "Numero de restricciones de Unicity: " << rUnicity.size() << endl;
//    cout << "Numero de restricciones Binarias de Unicity: " << rbUnicity.size() << endl;
//    
//     
//    
//    
//    
//    cout << "Prueba Mixer"  << endl;
//    
//    cout << "Solucion: " << resultadoMixer << endl;
//    
//
//    
//    
//    /*
//
//    cout << "Prueba Tokenizer"  << endl;
//    
//    vector<string> separado;
//    
//    Tokenize("g2",separado,":");
//    
//    for(int i = 0; i < separado.size() ; i++){
//        cout << "Letra Actividad: " << actLetter(separado.at(i)) << endl;
//        cout << "Numero estacion: " << numStation(separado.at(i)) << endl;
//    }
//     
//     
//     
//     */
//    
//    cout << resultadoMixer.size() << endl;
//    
//    cout << "numero de soluciones: " << temp.size() << endl;
//    for(int k =0;k<temp.size();k++){
//        cout << "Solucion " << k << endl;
//        temp.at(k).print();
//    }
//    
//    
//    
//    if(resultadoMixer.size()!=0){
//        finish = true;
//    }
//    
//    }
//    else
//    {
//        cout << "No hay solucion." << endl;
//    }
//    
//    heuristic = heuristic + gdc;
// 
//    
//
//    
//    
//    } // iteracion de tiempo de balanceo
//
//
//    // PRUEBAS
//
//    
//    
//    
//    
//    
//    
//    
//    return true;
//
//}
//
//bool pruebas(int argc, const char **argv){
// //   
// ////   VectorBR vBR (4, "a1,b1");
// ////   VectorBR vBR1 (4, "a1,b1");
//	////VectorBR sol(4, "a1");
//	////long long * aux3;
//	////aux3 = &vBR.Br.at(0);
//
// ////   vector<long long> aux = vBR.Br.at(0);
//	////vector<long long> aux1 = vBR.Br.at(1);
//
//	////cout << aux.size() << endl;
// //   
//	////cout << "----------------------------------------" << endl;
//	////cout << "Binary To decimal" << endl;
//	////long long t1;
//	////t1 = strtoull("10000000000000000000000000000000000000000000000000000000001", 0, 2);
//	////cout << t1 << endl;
//	////cout << "----------------------------------------" << endl;
//	////cout << "act To decimal" << endl;
//	////long long z1 = hashAuxll("a");
//	////cout << hashAuxll("a") << endl;;
//	////cout << dec2bin(hashAuxll("a")) << endl;
//	////cout << hashAuxll("z") << endl;;
//	////cout << dec2bin(hashAuxll("z")) << endl;
//	////cout << "Decimal To Binary" << endl;
//	////string t2;
//	////unsigned long long t3 = 1125899906842624;
//	////t2 = dec2bin(t3);
//	////cout << t2 << endl;
//	////cout << "----------------------------------------" << endl;
//
//
//	////cout << "----------------------------------------" << endl;
//
//	////aux.print();
//
//	////cout << "----------------------------------------" << endl;
//
//	////aux1.print();
//	////vBR.Br.at(0).NTable;
//    cout << "--------------------------------------------------------------------------------------------------" << endl;
//	int numStations = 4;
//	
//	//declare hashtable
//	//int hashTable_size = 1073741824/4;
//	//bool* hashTable = new bool[hashTable_size];
//
//	unsigned int vec1[1024];
//	unsigned int vec2[512];
//	unsigned int sol[225000]; //225,000 -> 56,250 restricciones de 4 estaciones
//
//	int vec1_size = 0;
//	unsigned int * ptr1 = vec1;
//	vec1_size = StrRestr2BinVector(4, "a0:b0:c0:d0,a0:b2:c1:d3,a1:b1:c2:d2", ptr1);
//	printf("\n\nTamano vector: %d \n\n", vec1_size);
//
//
//	int vec2_size = 0;
//	unsigned int * ptr2 = vec2;
//	vec2_size = StrRestr2BinVector(4, "a0:b0:c0:d0,z0", ptr2);
//	printf("\n\nTamano vector: %d \n\n", vec2_size);
//
//	printf("\n\n Vector 1\n\n");
//	//printArrayPointer(ptr1, vec1_size);
//	printf("\n\n Vector 2\n\n");
//	//printArrayPointer(ptr2, vec2_size);
//
//	printf("\n\n Vector Solucion antes de integracion \n\n");
//	unsigned int * ptr3 = sol;
//	//printArrayPointer(ptr3, (vec1_size * vec2_size)/4);
//
//	
//	runTest(argc, (const char **)argv, ptr_cont, numStations, ptr1,vec1_size, ptr2,vec2_size, ptr3);
//	printf("\n\n Vector Solucion despues de integracion \n\n");
//	//printArrayPointer(ptr3, (vec1_size * vec2_size) / 4);
//	std::map<string, bool> mapTable;
//
//	int j = 0;
//	int solution_cont = 0;
//	unsigned int no_dup_sol[2000];
//	string hash_key;
//	while (j < 840) {
//		hash_key.append(std::to_string(*(sol+j)));
//		hash_key.append(std::to_string(*(sol+j+1)));
//		hash_key.append(std::to_string(*(sol+j+2)));
//		hash_key.append(std::to_string(*(sol+j+3)));
//		cout << "hash_key " << j / 4 << " : " << hash_key << endl;
//
//		if (!mapTable[hash_key]) {
//			mapTable[hash_key] = true;
//			no_dup_sol[solution_cont + 0] = sol[j + 0];
//			no_dup_sol[solution_cont + 1] = sol[j + 1];
//			no_dup_sol[solution_cont + 2] = sol[j + 2];
//			no_dup_sol[solution_cont + 3] = sol[j + 3];
//			solution_cont+=4;
//
//
//
//		}
//
//		j += 4;
//		hash_key = "";
//	}
//
//	cout << "No duplicates: " << solution_cont/4 << endl;
//
//
//
//
//
//	cout << "--------------------------------------------------------------------------------------------------" << endl;
//	////si funciona inicializar en ceros una tabla con el constructyor que usa numero de estaciones
//	//BinaryRestriction aaa(4);
//	//aaa.print();
//	//cout << "--------------------------------------------------------------------------------------------------" << endl;
//
// //   cout << "VALID: " << aux.valid() << endl;
// //   
// //   vector<BinaryRestriction> solution;
// //   BinaryMixer mix;
// //   
// //   cout << vBR.Br.size() << endl;
// //   cout << vBR1.Br.size() << endl;
// //   
// //   solution = mix.mix(vBR.Br,vBR1.Br);
// //   cout << "--------------------------------------------------------------------------------------------------" << endl;
// //   
// //   cout << solution.size() << endl;
// //   
// //   for (int i=0;i<solution.size();i++){
// //       cout << "--------------------------------------------------------------------------------------------------" << endl;
// //       solution.at(i).print();
// //       cout << "llave: " << solution.at(i).HashKey()<< endl;
// //   }
//
//    return true;
//}
//
bool ExeVS() {

	cout << "TEST" << endl;

	return true;

}


bool executeBool(){
      
    cout << "TEST" << endl;
    
    cout << "READING FILE..." << endl;
 
    //Verify file existence
    
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/BOWMAN8.IN2";
    //Estaciones a verificar: 3 y 5
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JACKSON.IN2";
    //Estaciones a verificar: 3,4 y 5
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JAESCHKE.IN2";
    //Estaciones a verificar: 3,4,5 y 6 
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MANSOOR.IN2";
    //Estaciones a verificar: 3 y 4
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MERTENS.IN2";
    //Estaciones a verificar: 3 y 5
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/LASCURAIN.IN2";
    
    //Estaciones a verificar: 
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MITCHELL.IN2";
    
    //Estaciones a verificar: 
	string filename = "C:\\Users\\FERNYBOY\\Desktop\\Data_BL\\LASCURAIN.IN2";
    
    
    
    
    bool exist = exist_test(filename);
    
    cout << "File existence: " << exist << endl;
    

    //Create Tables
    //1. ForwardTimes, BackwardTimes
    //2. Precedence Table

    GraphCreator gc;

    Graph * graph = gc.createGraph(filename);
    graph->printGraph();
    cout << "--------------------------------------------------------------------------------------------------" << endl;      
    cout << "\n" << "Artificial Destination: " << graph->artificialDestination << endl;
    cout << "Artificial Origin: " << graph->artificialOrigin << "\n " << endl;
    
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "Max common divisor: ";
    int gdc = graph->ActivityDurationsGDC();
    cout << gdc <<  endl;
    
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "Graph Total Duration: " << graph->getTotalDuration() << endl;
    
    
    // Determine number of Stations for the desired BALANCING.
    // User input.
   
    int numberStations = 4;
    
    
    // Create Assembly Line Model
    // Solve Heuristic for this number of stations. 
    // CHANGE: I have to change this so the program start on the first
    // possible balancing and then iterate in increments of the minimum common denominator of the stations duration.
    
    
    AssemblyLine * al = new AssemblyLine(numberStations);
    Heuristic * op=new Heuristic(graph,al);
    op->solve();
    double heuristic=op->getHeuristic();
    cout << "Heuristic Balancing Time: " << heuristic << endl;
    
    //Change the value of heuristic for the ideal balancing time. The program iterates upwards in variations of the size of the 'Max common divisor'
    heuristic = graph->getTotalDuration()/numberStations;
    heuristic = ceil(heuristic);
    cout << "Ideal Balancing Time: " << heuristic << endl;
    
    //heuristic = 2872;
    
    //Program starts here...
    
    //Flag for Balancing iteration loop
    bool finish = false;
    
    //Balancing Time iteration loop
    while(!finish){
        
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "\n\n"<< endl;
    cout << "Tiempo de Balanceo: " << heuristic << endl;
    cout << "\n\n"<< endl;
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    
    
    // Create Simplifier for model
    
    Simplifier s(graph,heuristic,numberStations);
    

    
    //esta bandera me va a ayudar a determinar si hay alguna interseccion vacia
    // al momento de asignar actividades a estaciones.
    // significa que no es possible este balanceo y tengo que iterar

    AllCandidateStations * cand= s.simplify();
    
    if(cand->isFeasible())
    {
          
    // Binary Search for a lower limit
    // Determine which is the objective of this segment of code that simplifies the model
    
    double lowerLimit, upperLimit, middleValue;
    lowerLimit= graph->getTotalDuration()/numberStations;
    lowerLimit= round(lowerLimit,true);
    upperLimit=heuristic;

    double minValue,maxValue;

    while(lowerLimit<=upperLimit)
    {
            middleValue = round((lowerLimit+upperLimit)/2,true);
            s.setHeuristic(middleValue);
            cand = s.simplify();

            if(cand->isFeasible())
            {
                    upperLimit=middleValue-1;
            }
            else
            {
                    lowerLimit=middleValue+1;
            }

            delete cand;
    }

    cand = s.simplify();
    // If it is feasible return this one
    if(cand->isFeasible())
    {
            lowerLimit=middleValue;
    }
    // If it is not, return next (+1)
    else
    {
            lowerLimit=middleValue+1;
    }

    minValue=lowerLimit;
    maxValue=heuristic;

    // Step 3: Finished getting lower bound


    s.setHeuristic(heuristic);
    cand= s.simplify();

    // Step 4: Finished simplifying model
		
    cout << "Activity Information Table" << endl;
    graph->printActivityColletion();
    
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "Candidate stations for each activity: " << endl;
    cand->print();
    
    //Generate Model and Restrictions for Model
    RestrictionGenerator * r = new RestrictionGenerator(minValue,maxValue,cand,graph,al);
    Model * model= r->getModel();
    
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "------RESTRICCIONES----- \n\n" << endl;
    
    model->print();
    
    cout << "\n ------TABLA UNIVERSO----- \n" << endl;
    int** tablaUniverso = cand->generateTable();
    
    // "precedenceR" es un arreglo de objetos (PrecedenceRestrictions) que vamos a usar para estraer las ecuaciones booleanas
    // de las restricciones que si son validas, es decir que generan una restriccion.
    vector<PrecedenceRestriction *> arrPrecedence = model->getPrecedenceRestrictions();
    
    // Generar restricciones de precedencia negadas
        /*
         * 1. Un metodo que extraiga de la restriccion la combiancion de letras que no se puede
         * 2. Determinar la negacion de esta expression
         * 3. Crear un arreglo de string
         * 4. Guardar las restricciones generadas en este arreglo de strings
         */
    
    /*
     * La idea va a ser recorrer la tabla del universo de variables en la columna de origen hacia abajo y para cada uno de los renglones
     * Verificar que no exista un valor possible anterior en estacion para el actual.
     * 
     
     */
    
     cout << "\n\n ------RESTRICCIONES PRECEDENCIA BOOLEANAS----- \n\n" << endl;
    
    //formato correcto
    vector<string> rPrecedence;
    
    
    int origin, staOrigin;
    
    int dest, staDestination;
    int j = numberStations-1;
    int jDest = 0;
    bool flag = false;
    
    for(int i=0; i<arrPrecedence.size();i++){
        
        origin = arrPrecedence.at(i)->getOrigin();
        dest = arrPrecedence.at(i)->getDestination();
        
        cout << "ERROR EN PRECEDENCIA" << endl;
        cout << "Origen: " << origin << endl;
        cout << "Destino: " << dest << endl;
        
        while(j>0){
            //[estacion][actividad]
            if(tablaUniverso[j][origin]==1){
                staOrigin=j;
                flag = true;
            }
            
            
            while(flag && jDest<numberStations){
            //[estacion][actividad]
            if(tablaUniverso[jDest][dest]==1){
                if(j>jDest){
                    cout << "j: " << j << endl;
                    cout << "JDest: " << jDest << endl;
                    
                    staDestination=jDest;
                    string temp = ""; 
                    string temp1 = getNegation(origin, staOrigin,tablaUniverso, numberStations);
                    cout << "NEGACION ORIGIN: " << temp1 << endl;
                    string temp2 = getNegation(dest, staDestination,tablaUniverso, numberStations);
                    cout << "NEGACION DEST: " << temp2 <<endl;
                    
                    temp += temp1;
                    temp += ",";
                    temp += temp2;

                    cout << temp << endl;

                    // agregar a arreglo de restricciones de Precedencia
                    rPrecedence.push_back(temp);

                }
            }
            jDest++;
            }
            jDest = 0;
            j--;
            flag=false;

        }
        j = numberStations-1;
        
    }
    
    cout << "\n\n ------RESTRICTIONS UNICITY BOOLEANAS----- \n\n" << endl;
    
    
    
    
    vector<string> rUnicity = model->getUnicityRestrictions();
    
    string toString;
    int stationUnicity;
    for(int ii=0; ii< cand->getNumberActivities(); ii++){
        if(cand->getCandStationsForActivity(ii)->hasOnlyOneCandidate()){
                string UnicityConstr;
                toString = invHashAux(ii);
                UnicityConstr.append(toString);
                UnicityConstr.append(cand->getCandStationsForActivity(ii)->getCandidatesExtra());
                rUnicity.push_back(UnicityConstr);
            
        }
    }
    
    
    


    
    
    for(int i = 0; i< rUnicity.size() ;i++){
            cout << rUnicity.at(i) << endl;
    }
    
    //cand tiene el numero de actividades de ahi puedo generar la ultima restriccion
    // number of stations numberStations
    

    
            
    
    
    cout << "\n\n ------RESTRICTIONS DURACION BOOLEANAS----- \n" << endl;
    
    int numRecDuration = model->getNumberEquations();
    
    

    vector<Equation *> equations = model->getVectorEquations();

    vector<vector<DurationNode>> groupDN;
    
    for(int w=0; w<numRecDuration ; w++){
        
        vector<DurationNode> arr;
        int y= 0;
        Variable * aux;
        int balanceDuration = heuristic - equations[w]->getConstantValue();

        while(y<equations[w]->getNumVariables()){
            
            aux = equations[w]->getVariable(y);
            DurationNode * DN = new DurationNode( aux->getDuration(),aux->getActSta(), balanceDuration);  
            //Add Duration Node to Vector
            arr.push_back(*DN);
            y = y+1;
        }
        
        //Add Vector of Duration Nodes to array of restrictions.
        groupDN.push_back(arr);
    }
    
   
    // Verificar la creacion de los nodos de duracion.
    
    int size = groupDN.size();

    vector<DurationNode> arr;
    

    for(int i =0; i<size ; i++){      
        
        
        arr = groupDN[i];
        
        //esta vacio
        if(arr.size() != 0){
            groupDN[i] = MergeSort(arr);

            int numDNs= groupDN[i].size();
            //cout << "\n \n";

            for(int j =0; j<numDNs ; j++){
                groupDN[i][j].setId(j);

                // Imprimir los nodos ordenados.
                /*
                cout << "Nodo " << j << endl;
                cout<< "Duracion: " << groupDN[i][j].getDuration() << endl;
                cout<< "Restriction: " << groupDN[i][j].getRestriction() << endl;
                cout<< "Id: " << groupDN[i][j].getId() <<endl;
                cout<< "BalanceDuration: " << groupDN[i][j].getBalanceDuration() <<endl;
                cout << "\n";
                */
            }
            
        }

        
        
        
    }

    
    vector<string> vectorRest ;
    vector<string> * vectorR = &vectorRest;
    
    cout << groupDN.size() << endl;
    for(int j =0; j<groupDN.size(); j++){
        
        vector<DurationNode> * vectorDN = &groupDN[j];
        if(vectorDN->size() != 0){
                DurationTree * tree = new DurationTree(vectorDN, vectorR, groupDN[j][0].getBalanceDuration());
                tree->buildTree();
        }

        
    }   
    
    
    cout << "Restricciones Generadas:" << vectorR->size() << endl;
    
    //
    //for(int i = 0; i < vectorR->size() ; i++){
    //    //cout << vectorR->at(i) << endl;
    //}
    
    cout << "Negacion de Restricciones Generadas:" << vectorR->size() << endl;
    
    for(int i = 0; i < vectorR->size() ; i++){
        string negC = constraintNeg(vectorR->at(i), tablaUniverso, numberStations);
        vectorR->at(i) = negC;
        //cout << vectorR->at(i) << endl;
    }
    
    
    cout << "Integracion Booleana" << endl;
    
    
    
    
    
    BinaryMixer bMixer;

	//TIME
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	auto duration = 0;

	//Inicia Tiempo 
	t1 = std::chrono::high_resolution_clock::now();
    
    Mixer * mixer = new Mixer();
    
    //Primera restriccion entra al mixer. El mixer es donde se van a realizar las operaciones tipo AND.
    string resultadoMixer = vectorR->at(0);
    
    VectorBR auxRtoB(numberStations, vectorR->at(0));
    vector<BinaryRestriction> temp = auxRtoB.Br;
    
    cout << "size" << endl;
    cout << temp.size() << endl;
    
    
    cout << "\n Primera Restriccion: " << resultadoMixer << endl;
    
    
    string tempM;
    int contador= 0;
    int totalRestrictions = vectorR->size() + rPrecedence.size() + rUnicity.size();
    
    cout << "Total Restricciones: " << totalRestrictions;

    cout << "\n\n ------RESTRICCIONES PRECEDENCIA BOOLEANAS----- \n\n" << endl;


    vector<VectorBR> rbPrecedence;
    
    //Binarias
    vector<VectorBR> rbDuracion;
        
    //Binarias
    vector<VectorBR> rbUnicity;

	//TIME
	std::chrono::high_resolution_clock::time_point t3;
	std::chrono::high_resolution_clock::time_point t4;
	auto duration2 = 0;

	//Inicia Tiempo 



    for(int i = 0; i < rPrecedence.size() ; i++){
		
        //convertir las restricciones a conjunte de restricciones binarias
        VectorBR auxRtoB(numberStations, rPrecedence.at(i));
        rbPrecedence.push_back(auxRtoB);

		//cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
		t3 = std::chrono::high_resolution_clock::now();
		temp = bMixer.mix(temp, rbPrecedence.at(i).Br);
		t4 = std::chrono::high_resolution_clock::now();
		duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
		//cout << duration2 << endl;
		cout << temp.size() << endl;

        contador++;


    }
    
    
    for(int i = 1; i < vectorR->size(); i++){
        //convertir las restricciones a conjunte de restricciones binarias
        VectorBR auxRtoB(numberStations, vectorR->at(i));
        rbDuracion.push_back(auxRtoB);


		//cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
		t3 = std::chrono::high_resolution_clock::now();
		temp = bMixer.mix(temp, rbDuracion.at(i - 1).Br);
		t4 = std::chrono::high_resolution_clock::now();
		duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
		//cout <<  duration2 << endl;
		cout << temp.size() << endl;
        
       
        contador++;

    }

    


    
    for(int i = 0; i < rUnicity.size() ; i++){
        //convertir las restricciones a conjunte de restricciones binarias
        VectorBR auxRtoB(numberStations, rUnicity.at(i));
        rbUnicity.push_back(auxRtoB);

		//cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
		t3 = std::chrono::high_resolution_clock::now();
		temp = bMixer.mix(temp, rbUnicity.at(i).Br);
		t4 = std::chrono::high_resolution_clock::now();
		duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
		//cout << duration2 << endl;
		cout << temp.size() << endl;

        contador++;
       
    }
    

    
    
    
        //imprimir numero de restricciones
    cout << "Numero de restricciones de Duracion: " << vectorR->size() << endl;
    cout << "Numero de restricciones Binarias de Duracion: " << rbDuracion.size() << endl;

    //imprimir numero de restricciones
    cout << "Numero de restricciones de Precedencia: " << rPrecedence.size() << endl;
    cout << "Numero de restricciones Binarias de Precedencia: " << rbPrecedence.size() << endl;

    //imprimir numero de restricciones
    cout << "Numero de restricciones de Unicity: " << rUnicity.size() << endl;
    cout << "Numero de restricciones Binarias de Unicity: " << rbUnicity.size() << endl;
    


    cout << "numero de soluciones: " << temp.size() << endl;
    for(int k =0;k<temp.size();k++){
        cout << "Solucion " << k << endl;
        temp.at(k).print();
    }
    
    
    
//    if(resultadoMixer.size()!=0){
//        finish = true;
//    }
    if(temp.size()!=0){
        finish = true;

		//Tiempo Fin
		t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		cout << "Tiempo: " << duration << endl;


    }
    
    
    }
    else
    {
        cout << "No hay solucion." << endl;
    }
    
    heuristic = heuristic + gdc;
 
    

    
    
    } // iteracion de tiempo de balanceo


    // PRUEBAS

    
    return true;

}


bool parallelExe(int argc, const char **argv){
	
    cout << "TEST" << endl;
    
    cout << "READING FILE..." << endl;
 
    //Verify file existence
    
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/BOWMAN8.IN2";
    //Estaciones a verificar: 3 y 5
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JACKSON.IN2";
    //Estaciones a verificar: 3,4 y 5
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JAESCHKE.IN2";
    //Estaciones a verificar: 3,4,5 y 6 
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MANSOOR.IN2";
    //Estaciones a verificar: 3 y 4
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MERTENS.IN2";
    //Estaciones a verificar: 3 y 5
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/LASCURAIN.IN2";
    //Estaciones a verificar: 
    //string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MITCHELL.IN2";

	// ROZIEG.IN2
    
    //Read from file
	string filename = "C:\\Users\\FERNYBOY\\Desktop\\Data_BL\\JAESCHKE.IN2";
    bool exist = exist_test(filename);
    cout << "File existence: " << exist << endl;
    

    //Create Tables
    //1. ForwardTimes, BackwardTimes
    //2. Precedence Table
    GraphCreator gc;
    Graph * graph = gc.createGraph(filename);
    graph->printGraph();
    cout << "--------------------------------------------------------------------------------------------------" << endl;      
    cout << "\n" << "Artificial Destination: " << graph->artificialDestination << endl;
    cout << "Artificial Origin: " << graph->artificialOrigin << "\n " << endl;
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "Max common divisor: ";
    int gdc = graph->ActivityDurationsGDC();
    cout << gdc <<  endl;
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "Graph Total Duration: " << graph->getTotalDuration() << endl;
    
    
    // Determine NUMBER OF STATIONS for the desired BALANCING.
    // User input.
	//I could use c.in
    int numberStations = 5;
   
    // Create Assembly Line Model
    // Solve Heuristic for this number of stations. Returns an working solution for the balancing time. 
    AssemblyLine * al = new AssemblyLine(numberStations);
    Heuristic * op=new Heuristic(graph,al);
    op->solve();
    double heuristic=op->getHeuristic();
    cout << "Heuristic Balancing Time: " << heuristic << endl;
    
    //Change the value of heuristic for the ideal balancing time. The program iterates upwards in variations of the size of the 'Max common divisor'
    heuristic = graph->getTotalDuration()/numberStations;
    heuristic = ceil(heuristic);
    cout << "Ideal Balancing Time: " << heuristic << endl;
    
    
    //Program starts here...

    /////////////////////////////////////
	////BALANCING TIME ITERATION LOOP////
	/////////////////////////////////////

    //finish: Flag for Balancing iteration loop
    bool finish = false;

    while(!finish){

    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "\n\n\nBalancing Time: \n\n\n" << heuristic << endl;
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    
    // Create Simplifier for model
    Simplifier s(graph,heuristic,numberStations);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//START: Need better comments
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //esta bandera me va a ayudar a determinar si hay alguna interseccion vacia
    // al momento de asignar actividades a estaciones.
    // significa que no es possible este balanceo y tengo que iterar

    AllCandidateStations * cand= s.simplify();

    if(cand->isFeasible())
    {
          
    // Binary Search for a lower limit
    // Determine which is the objective of this segment of code that simplifies the model
    
    double lowerLimit, upperLimit, middleValue;
    lowerLimit= graph->getTotalDuration()/numberStations;
    lowerLimit= round(lowerLimit,true);
    upperLimit=heuristic;

    double minValue,maxValue;

    while(lowerLimit<=upperLimit)
    {
            middleValue = round((lowerLimit+upperLimit)/2,true);
            s.setHeuristic(middleValue);
            cand = s.simplify();

            if(cand->isFeasible())
            {
                    upperLimit=middleValue-1;
            }
            else
            {
                    lowerLimit=middleValue+1;
            }

            delete cand;
    }

    cand = s.simplify();
    // If it is feasible return this one
    if(cand->isFeasible())
    {
            lowerLimit=middleValue;
    }
    // If it is not, return next (+1)
    else
    {
            lowerLimit=middleValue+1;
    }

    minValue=lowerLimit;
    maxValue=heuristic;

    // Step 3: Finished getting lower bound

    s.setHeuristic(heuristic);
    cand= s.simplify();

    // Step 4: Finished simplifying model
		
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//END: Need better comments
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    cout << "Activity Information Table" << endl;
    graph->printActivityColletion();
    
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "Candidate stations for each activity: " << endl;
    cand->print();
    
    //Generate Model and Restrictions for Model
    RestrictionGenerator * r = new RestrictionGenerator(minValue,maxValue,cand,graph,al);
    Model * model= r->getModel();
    
    cout << "--------------------------------------------------------------------------------------------------" << endl;
    cout << "------RESTRICCIONES----- \n\n" << endl;
    
    model->print();
    
    cout << "\n ------TABLA UNIVERSO----- \n" << endl;
    int** tablaUniverso = cand->generateTable();
    
    // "precedenceR" es un arreglo de objetos (PrecedenceRestrictions) que vamos a usar para extraer las ecuaciones booleanas
    // de las restricciones que si son validas, es decir que generan una restriccion.
    vector<PrecedenceRestriction *> arrPrecedence = model->getPrecedenceRestrictions();
    
    // Generar restricciones de precedencia negadas
        /*
         * 1. Un metodo que extraiga de la restriccion la combiancion de letras que no se puede
         * 2. Determinar la negacion de esta expression
         * 3. Crear un arreglo de string
         * 4. Guardar las restricciones generadas en este arreglo de strings
         */
    
    /*
     * La idea va a ser recorrer la tabla del universo de variables en la columna de origen hacia abajo y para cada uno de los renglones
     * Verificar que no exista un valor possible anterior en estacion para el actual.
     * 
     
     */
    
	 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //PRECEDENCE CONSTRAINT GENERATION
	 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

     cout << "\n\n ------RESTRICCIONES PRECEDENCIA BOOLEANAS----- \n\n" << endl;
    //formato correcto
	//Esto puede ser una funcion fuera de MAIN
    vector<string> rPrecedence;
    
    int origin, staOrigin;
    int dest, staDestination;
    int j = numberStations-1;
    int jDest = 0;
    bool flag = false;
    
    for(int i=0; i<arrPrecedence.size();i++){
        
        origin = arrPrecedence.at(i)->getOrigin();
        dest = arrPrecedence.at(i)->getDestination();
        
        cout << "ERROR EN PRECEDENCIA" << endl;
        cout << "Origen: " << origin << endl;
        cout << "Destino: " << dest << endl;
        
        while(j>0){
            //[estacion][actividad]
            if(tablaUniverso[j][origin]==1){
                staOrigin=j;
                flag = true;
            }
            
            
            while(flag && jDest<numberStations){
            //[estacion][actividad]
            if(tablaUniverso[jDest][dest]==1){
                if(j>jDest){
                    cout << "j: " << j << endl;
                    cout << "JDest: " << jDest << endl;
                    
                    staDestination=jDest;
                    string temp = ""; 
                    string temp1 = getNegation(origin, staOrigin,tablaUniverso, numberStations);
                    cout << "NEGACION ORIGIN: " << temp1 << endl;
                    string temp2 = getNegation(dest, staDestination,tablaUniverso, numberStations);
                    cout << "NEGACION DEST: " << temp2 <<endl;
                    
                    temp += temp1;
                    temp += ",";
                    temp += temp2;
                    cout << temp << endl;
                    // agregar a arreglo de restricciones de Precedencia
                    rPrecedence.push_back(temp);

                }
            }
            jDest++;
            }
            jDest = 0;
            j--;
            flag=false;

        }
        j = numberStations-1;
        
    }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//UNICITY CONSTRAINT GENERATION
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    cout << "\n\n ------RESTRICTIONS UNICITY BOOLEANAS----- \n\n" << endl;
    
    vector<string> rUnicity = model->getUnicityRestrictions();
    
    string toString;
    int stationUnicity;
    for(int ii=0; ii< cand->getNumberActivities(); ii++){
        if(cand->getCandStationsForActivity(ii)->hasOnlyOneCandidate()){
                string UnicityConstr;
                toString = invHashAux(ii);
                UnicityConstr.append(toString);
                UnicityConstr.append(cand->getCandStationsForActivity(ii)->getCandidatesExtra());
                rUnicity.push_back(UnicityConstr);
            
        }
    }

    for(int i = 0; i< rUnicity.size() ;i++){
            cout << rUnicity.at(i) << endl;
    }
    
    // cand tiene el numero de actividades de ahi puedo generar la ultima restriccion
    // number of stations numberStations
    

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//DURATION CONSTRAINT GENERATION
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    cout << "\n\n ------RESTRICTIONS DURACION BOOLEANAS----- \n" << endl;
    
    int numRecDuration = model->getNumberEquations();
    vector<Equation *> equations = model->getVectorEquations();
    vector<vector<DurationNode>> groupDN;
    
    for(int w=0; w<numRecDuration ; w++){
        vector<DurationNode> arr;
        int y= 0;
        Variable * aux;
        int balanceDuration = heuristic - equations[w]->getConstantValue();

        while(y<equations[w]->getNumVariables()){
            aux = equations[w]->getVariable(y);
            DurationNode * DN = new DurationNode( aux->getDuration(),aux->getActSta(), balanceDuration);  
            //Add Duration Node to Vector
            arr.push_back(*DN);
            y = y+1;
        }
        
        //Add Vector of Duration Nodes to array of restrictions.
        groupDN.push_back(arr);
    }
    
    // Verificar la creacion de los nodos de duracion.
    int size = groupDN.size();
    vector<DurationNode> arr;
    
    for(int i =0; i<size ; i++){      
        arr = groupDN[i];
        
        //esta vacio
        if(arr.size() != 0){
            groupDN[i] = MergeSort(arr);
            int numDNs= groupDN[i].size();
            //cout << "\n \n";

            for(int j =0; j<numDNs ; j++){
                groupDN[i][j].setId(j);
                // Imprimir los nodos ordenados.
                /*
                cout << "Nodo " << j << endl;
                cout<< "Duracion: " << groupDN[i][j].getDuration() << endl;
                cout<< "Restriction: " << groupDN[i][j].getRestriction() << endl;
                cout<< "Id: " << groupDN[i][j].getId() <<endl;
                cout<< "BalanceDuration: " << groupDN[i][j].getBalanceDuration() <<endl;
                cout << "\n";
                */
            }
        }

    }

    
    vector<string> vectorRest ;
    vector<string> * vectorR = &vectorRest;
    
    cout << groupDN.size() << endl;
    for(int j =0; j<groupDN.size(); j++){
        vector<DurationNode> * vectorDN = &groupDN[j];
        if(vectorDN->size() != 0){
                DurationTree * tree = new DurationTree(vectorDN, vectorR, groupDN[j][0].getBalanceDuration());
                tree->buildTree();
        }

        
    }
  
    // NUMBER OF DURATION RESTRICTIONS GENERATED
    cout << "Restricciones Generadas:" << vectorR->size() << endl;
    
    
    //for(int i = 0; i < vectorR->size() ; i++){
    //    //cout << vectorR->at(i) << endl;
    //}
    
        /*
    cout << "Prueba de negacion de una restriccion: " << endl;
    
    string negC = constraintNeg("a0", tablaUniverso, numberStations);
    
    cout << "Negacion: " << negC << endl;
    
    cout << cand->getCandStationsForActivity(9)->getCandidatesExtra() << endl;
    //cand->getCandStationsForActivity(0)->hasOnlyOneCandidate()
         * */
    
    
    cout << "Negacion de Restricciones Generadas:" << vectorR->size() << endl;
    
    for(int i = 0; i < vectorR->size() ; i++){
        string negC = constraintNeg(vectorR->at(i), tablaUniverso, numberStations);
        vectorR->at(i) = negC;
        //cout << vectorR->at(i) << endl;
    }
    
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CONSTRAINT BINARY INTEGRATION IN PARALLEL
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//PARALLEL CALL FUNCTION: runtest()


	cout << "Integracion Booleana" << endl;

    //BinaryMixer bMixer;
    //Mixer * mixer = new Mixer();

	//////////////////////////////
	/////Variable Declaration/////
	//////////////////////////////
	//const int width = 10;
	//const unsigned long long int height = 4294967295;
	//int** ptr6 = new int*[width];
	//for (int i = 0; i < width; ++i)
	//	ptr6[i] = new int[height];

	//unsigned int * ptr7 = new unsigned int[10];
	//memset(ptr7, 0, 10 * sizeof(int));

	//int** ptr8 = new int*[width];
	//for (int i = 0; i < width; ++i)
	//	ptr8[i] = new int[height];

	//unsigned int * ptr9 = new unsigned int[10];
	//memset(ptr9, 0, 10 * sizeof(int));


	const unsigned long long int sol_vector_size = 4294967295;
	const unsigned long long int rest_vector_size = 10000;



	//Auxiliar int arrays
	//unsigned int S1[1000];
	//unsigned int S2[75000];
	unsigned int ARR1[rest_vector_size]; //225,000 -> 56,250 restricciones de 4 estaciones
	//unsigned int no_dup_sol[75000];
	//Pointers to int arrays
	unsigned int * ptr1 = new unsigned int[rest_vector_size];
	unsigned int * ptr2 = new unsigned int[sol_vector_size];
	unsigned int * ptr3 = ARR1;
	unsigned int * ptr4 = new unsigned int[sol_vector_size];
	
	//inicializar en ceros
	memset(ptr1, 0, rest_vector_size * sizeof(int));
	memset(ptr3, 0, rest_vector_size * sizeof(int));
	memset(ptr2, 0, sol_vector_size * sizeof(int));
	memset(ptr4, 0, sol_vector_size * sizeof(int));

	//In order to keep track how many elements there are in certain array
	//Number of occupied space in the array
	int S1_size = 0;
	int S2_size = 0;
	int ARR1_size = 0;
	unsigned long long int NoDup_size = 0;

	//AUXILIAR Counters
	unsigned long long int  jj = 0;
	unsigned long long int solution_cont = 0;
	unsigned long long int cont_GPU = 0;
	unsigned long long int * ptr_cont;
	ptr_cont = &cont_GPU;

	//HASH TABLE
	string hash_key;
	string null_hash_key;
	for (int i = 0; i < numberStations; i++) {
		null_hash_key.append("0");
	}
	std::map<string, bool> mapTable;
	mapTable[null_hash_key] = true;





	//unsigned long long int hash_key = 0;
	//unsigned long long int null_hash_key = 0;
	//unsigned long long int control_location = 1;
	////bool hashmap = n
	//bool * ptr_hashmap = new bool[2000000000];
	//memset(ptr_hashmap, false, sizeof(bool)*2000000000);


	/////////////////////////////////////////////////////
	////////////////HASHMAP//////////////////////////////
	/////////////////////////////////////////////////////


	//TIME
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	auto duration = 0;

	// S1: Array that will store the temporal solutions.
	// S2: Array that will store the temporal solution
	// ARR1: Array that will store the new constraint that will be merged with the temporal solution.

	// Transform and save 1st constrain in S1.
	cout << vectorR->at(0) << endl;
	S1_size = StrRestr2BinVector(numberStations, vectorR->at(0), ptr1);
	cout << "Size Temp:  \n" << S1_size/numberStations << endl;

	printArrayPointer(ptr1, S1_size, numberStations);

	// Transform and save 2nd constrain in ARR1.
	cout << vectorR->at(1) << endl;
	ARR1_size = StrRestr2BinVector(numberStations, vectorR->at(1), ptr3);
	cout << "Size Restriction: \n" << ARR1_size / numberStations << endl;

	printArrayPointer(ptr3, ARR1_size, numberStations);

	//Inicia Tiempo 
	t1 = std::chrono::high_resolution_clock::now();
	
	//////////////////////////////////////
	/////EXECUTE PARALLEL INTEGRATION/////
	//////////////////////////////////////

	runTest(argc, (const char **)argv, ptr_cont ,numberStations, ptr1, S1_size, ptr3, ARR1_size, ptr2);
	printArrayPointer(ptr2, (S1_size*ARR1_size) / numberStations, numberStations);

	solution_cont = 0;
		while (jj < (*ptr_cont)*numberStations) {

			cout << "hashing status:  " << jj << "/" << ((*ptr_cont)*numberStations) << endl;
			
			for (int kk = 0; kk < numberStations; kk++) {
				hash_key.append(std::to_string(*(ptr2 + jj + kk)));
			}
			//cout << "hash_key " << jj / numberStations << " : " << hash_key << endl;

			if (!mapTable[hash_key]) {
				mapTable[hash_key] = true;

				for (int kk = 0; kk < numberStations; kk++) {
					*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
				}
				solution_cont += numberStations;
			}
			jj += numberStations;
			hash_key = "";
		}
	NoDup_size = solution_cont;
	

	cout << "Tamano TEMP: " << NoDup_size << endl;
	
	printArrayPointer(ptr4, NoDup_size, numberStations);
	//cout << "Number of solutions: " << NoDup_size << endl;
	
	//SUMA DE RESTRICCIONES NO ME ESTA DANDO EXACTA???
	int contador= 1;
	int totalRestrictions = vectorR->size() + rPrecedence.size() + rUnicity.size()-1;

	cout << "size P: " << rPrecedence.size() << endl;
	cout << "size D: " << vectorR->size() << endl;
	cout << "size U: " << rUnicity.size() << endl;
	
		///////////////////////////////////////////
		/////PRECEDENT RESTRICTION INTEGRATION/////
		///////////////////////////////////////////

	int cont_mix = 0;
	//for(int i = 0; i < rPrecedence.size() ; i++){
	while (cont_mix < rPrecedence.size() && NoDup_size>0) {
		//Reset values of ARR1 to zero.
		memset(ARR1, 0, sizeof(ARR1));

		*ptr_cont = 0;
		//Transform String::Constraint into Array[int]::Constraint
		ARR1_size = StrRestr2BinVector(numberStations, rPrecedence.at(cont_mix), ptr3);
		cout << "-----------------------------------------------------" << endl;
		cout << "RESTRICCION" << endl;
		//printArrayPointer(ptr3, ARR1_size, numberStations);
		cout << "-----------------------------------------------------" << endl;
		//Parallel Execution
		//cout << "elements en temp: " << NoDup_size << " \t Elements in REST: " << ARR1_size << endl;
		//cout << "Elements in solutions vector: " << (NoDup_size*ARR1_size) / numberStations;

		runTest(argc, (const char **)argv, ptr_cont, numberStations, ptr4, solution_cont, ptr3, ARR1_size, ptr2);

		solution_cont = 0;
		jj = 0;
		mapTable.clear();
		mapTable[null_hash_key] = true;
		cout << "elementos para hash: " << *ptr_cont << endl;
		while (jj < (*ptr_cont)*numberStations) {

			//if (jj % 10000000 == 0) {
				//cout << "hashing status:  " << jj << "/" << ((*ptr_cont)*numberStations) << endl;
			//}

			for (int kk = 0; kk < numberStations; kk++) {
				hash_key.append(std::to_string(*(ptr2 + jj + kk)));
			}
			//cout << "hash_key " << jj / numberStations << " : " << hash_key << endl;

			if (!mapTable[hash_key]) {
				mapTable[hash_key] = true;

				for (int kk = 0; kk < numberStations; kk++) {
					*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
				}
				solution_cont += numberStations;
			}
			jj += numberStations;
			hash_key = "";
		}
		NoDup_size = solution_cont;
		cout << "-----------------------------------------------------" << endl;
		cout << "SOLUTION" << endl;
		cout << "Number of solutions: " << NoDup_size / numberStations << endl;
		//printArrayPointer(ptr4, (NoDup_size), numberStations);



		cout << "-----------------------------------------------------" << endl;
		contador++;
		cout << "Iteracion " << contador << "/" << totalRestrictions << endl;

		cont_mix++;
	}

		///////////////////////////////////////////////
		/////////DURATION RESTRICTION INTEGRATION/////
		///////////////////////////////////////////////

		cont_mix = 2;

		//for (int i = 2; i < vectorR->size(); i++) {
		while(cont_mix < vectorR->size() && NoDup_size>0){
			//Transform String::Constraint into Array[int]::Constraint
			//Reset values of ARR1 to zero.
			memset(ARR1, 0, sizeof(ARR1));
			*ptr_cont = 0;
			ARR1_size = StrRestr2BinVector(numberStations, vectorR->at(cont_mix), ptr3);
			cout << "-----------------------------------------------------" << endl;
			cout << "RESTRICCION" << endl;
			//printArrayPointer(ptr3, ARR1_size, numberStations);
			cout << "-----------------------------------------------------" << endl;
			//Parallel Execution
			//cout << "elements en temp: " << NoDup_size << " \t Elements in REST: " << ARR1_size << endl;
			runTest(argc, (const char **)argv,ptr_cont, numberStations, ptr4, solution_cont, ptr3, ARR1_size, ptr2);

			solution_cont = 0;
			jj = 0;
			mapTable.clear();
			mapTable[null_hash_key] = true;
			cout << "elementos para hash: " << *ptr_cont << endl;
			while (jj < ((*ptr_cont)*numberStations)) {
				for (int kk = 0; kk < numberStations; kk++) {
					hash_key.append(std::to_string(*(ptr2 + jj + kk)));
				}

					//cout << "hash_key " << jj / numberStations << " : " << hash_key << endl;

				

				if (!mapTable[hash_key]) {
					mapTable[hash_key] = true;

					for (int kk = 0; kk < numberStations; kk++) {
						*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
					}
					solution_cont += numberStations;
				}
				jj += numberStations;
				hash_key = "";
			}
			NoDup_size = solution_cont;
			cout << "-----------------------------------------------------" << endl;
			cout << "SOLUTION" << endl;
			cout << "Number of solutions: " << NoDup_size/numberStations << endl;
			//printArrayPointer(ptr4, (NoDup_size), numberStations);
			cout << "-----------------------------------------------------" << endl;
			contador++;
			cout << "Iteracion " << contador << "/" << totalRestrictions << endl;

			cont_mix++;
		}



	////	/////////////////////////////////////////
	////	/////UNICITY RESTRICTION INTEGRATION/////
	////	/////////////////////////////////////////

		cont_mix =  0;

		//for (int i = 0; i < rUnicity.size(); i++) {
		while(cont_mix < rUnicity.size() && NoDup_size>0){
			//Transform String::Constraint into Array[int]::Constraint
			//Reset values of ARR1 to zero.
			memset(ARR1, 0, sizeof(ARR1));

			*ptr_cont = 0;
			ARR1_size = StrRestr2BinVector(numberStations, rUnicity.at(cont_mix), ptr3);
			cout << "-----------------------------------------------------" << endl;
			cout << "RESTRICCION" << endl;
			//printArrayPointer(ptr3, ARR1_size, numberStations);
			cout << "-----------------------------------------------------" << endl;
			//Parallel Execution
			//cout << "elements en temp: " << NoDup_size << " \t Elements in REST: " << ARR1_size << endl;
			runTest(argc, (const char **)argv, ptr_cont,numberStations, ptr4, solution_cont, ptr3, ARR1_size, ptr2);

			solution_cont = 0;
			jj = 0;
			mapTable.clear();
			mapTable[null_hash_key] = true;
			while (jj < (*ptr_cont)* numberStations) {
				for (int kk = 0; kk < numberStations; kk++) {
					hash_key.append(std::to_string(*(ptr2 + jj + kk)));
				}
				//cout << "hash_key " << jj / numberStations << " : " << hash_key << endl;

				if (!mapTable[hash_key]) {
					mapTable[hash_key] = true;

					for (int kk = 0; kk < numberStations; kk++) {
						*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
					}
					solution_cont += numberStations;
				}
				jj += numberStations;
				hash_key = "";
			}
			NoDup_size = solution_cont;
			cout << "-----------------------------------------------------" << endl;
			cout << "SOLUTION" << endl;
			cout << "Number of solutions: " << NoDup_size/numberStations << endl;
			//printArrayPointer(ptr4, (NoDup_size), numberStations);
			cout << "-----------------------------------------------------" << endl;
			contador++;
			cout << "Iteracion " << contador << "/" << totalRestrictions << endl;

			cont_mix++;
		}



		//If que controle cuando acabo
		if ((NoDup_size / numberStations) != 0) {

			//Tiempo Fin
			t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
			cout << "Tiempo: " << duration << endl;

			finish = true;

			cout << "-----------------------------------------------------" << endl;
			cout << "SOLUTION" << endl;
			cout << "Tiempo: " << duration << endl;
			cout << "Number of constraints:  " <<  totalRestrictions << endl;
			cout << "Number of solutions: " << NoDup_size / numberStations << endl;
			cout << "\nBalancing Time: " << heuristic << "\n\n" << endl;
			printArrayPointer(ptr4, (NoDup_size), numberStations);
			cout << "-----------------------------------------------------" << endl;


		}
		else {
			heuristic = heuristic + gdc;
		}

    } //if cand is feasible
    else
    {
        cout << "No hay solucion." << endl;
		heuristic = heuristic + gdc;
    }
    
    } // iteracion de tiempo de balanceo while


    
    return true;

}


bool parallelExeHash(int argc, const char **argv) {
	
	cout << "TEST" << endl;

	cout << "READING FILE..." << endl;

	//Verify file existence

	//string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/BOWMAN8.IN2";
	//Estaciones a verificar: 3 y 5
	//string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JACKSON.IN2";
	//Estaciones a verificar: 3,4 y 5
	//string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/JAESCHKE.IN2";
	//Estaciones a verificar: 3,4,5 y 6 
	//string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MANSOOR.IN2";
	//Estaciones a verificar: 3 y 4
	//string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MERTENS.IN2";
	//Estaciones a verificar: 3 y 5
	//string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/LASCURAIN.IN2";
	//Estaciones a verificar: 
	//string filename = "/Users/normalazo/Desktop/Juan Pablo copy/BalanceoDeLineas/Data_BL/MITCHELL.IN2";

	// ROZIEG.IN2

	//Read from file
	string filename = "C:\\Users\\FERNYBOY\\Desktop\\Data_BL\\LASCURAIN.IN2";
	bool exist = exist_test(filename);
	cout << "File existence: " << exist << endl;


	//Create Tables
	//1. ForwardTimes, BackwardTimes
	//2. Precedence Table
	GraphCreator gc;
	Graph * graph = gc.createGraph(filename);
	graph->printGraph();
	cout << "--------------------------------------------------------------------------------------------------" << endl;
	cout << "\n" << "Artificial Destination: " << graph->artificialDestination << endl;
	cout << "Artificial Origin: " << graph->artificialOrigin << "\n " << endl;
	cout << "--------------------------------------------------------------------------------------------------" << endl;
	cout << "Max common divisor: ";
	int gdc = graph->ActivityDurationsGDC();
	cout << gdc << endl;
	cout << "--------------------------------------------------------------------------------------------------" << endl;
	cout << "Graph Total Duration: " << graph->getTotalDuration() << endl;


	// Determine NUMBER OF STATIONS for the desired BALANCING.
	// User input.
	//I could use c.in
	int numberStations = 5;

	// Create Assembly Line Model
	// Solve Heuristic for this number of stations. Returns an working solution for the balancing time. 
	AssemblyLine * al = new AssemblyLine(numberStations);
	Heuristic * op = new Heuristic(graph, al);
	op->solve();
	double heuristic = op->getHeuristic();
	cout << "Heuristic Balancing Time: " << heuristic << endl;

	//Change the value of heuristic for the ideal balancing time. The program iterates upwards in variations of the size of the 'Max common divisor'
	heuristic = graph->getTotalDuration() / numberStations;
	heuristic = ceil(heuristic);
	cout << "Ideal Balancing Time: " << heuristic << endl;


	//Program starts here...

	/////////////////////////////////////
	////BALANCING TIME ITERATION LOOP////
	/////////////////////////////////////

	//finish: Flag for Balancing iteration loop
	bool finish = false;

	while(!finish){

	cout << "--------------------------------------------------------------------------------------------------" << endl;
	cout << "\n\n\nBalancing Time: \n\n\n" << heuristic << endl;
	cout << "--------------------------------------------------------------------------------------------------" << endl;

	// Create Simplifier for model
	Simplifier s(graph, heuristic, numberStations);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//START: Need better comments
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//esta bandera me va a ayudar a determinar si hay alguna interseccion vacia
	// al momento de asignar actividades a estaciones.
	// significa que no es possible este balanceo y tengo que iterar

	AllCandidateStations * cand = s.simplify();

	if(cand->isFeasible())
	{

	// Binary Search for a lower limit
	// Determine which is the objective of this segment of code that simplifies the model

	double lowerLimit, upperLimit, middleValue;
	lowerLimit = graph->getTotalDuration() / numberStations;
	lowerLimit = round(lowerLimit, true);
	upperLimit = heuristic;

	double minValue, maxValue;

	while (lowerLimit <= upperLimit)
	{
		middleValue = round((lowerLimit + upperLimit) / 2, true);
		s.setHeuristic(middleValue);
		cand = s.simplify();

		if (cand->isFeasible())
		{
			upperLimit = middleValue - 1;
		}
		else
		{
			lowerLimit = middleValue + 1;
		}

		delete cand;
	}

	cand = s.simplify();
	// If it is feasible return this one
	if (cand->isFeasible())
	{
		lowerLimit = middleValue;
	}
	// If it is not, return next (+1)
	else
	{
		lowerLimit = middleValue + 1;
	}

	minValue = lowerLimit;
	maxValue = heuristic;

	// Step 3: Finished getting lower bound

	s.setHeuristic(heuristic);
	cand = s.simplify();

	// Step 4: Finished simplifying model

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//END: Need better comments
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	cout << "Activity Information Table" << endl;
	graph->printActivityColletion();

	cout << "--------------------------------------------------------------------------------------------------" << endl;
	cout << "Candidate stations for each activity: " << endl;
	cand->print();

	//Generate Model and Restrictions for Model
	RestrictionGenerator * r = new RestrictionGenerator(minValue, maxValue, cand, graph, al);
	Model * model = r->getModel();

	cout << "--------------------------------------------------------------------------------------------------" << endl;
	cout << "------RESTRICCIONES----- \n\n" << endl;

	model->print();

	cout << "\n ------TABLA UNIVERSO----- \n" << endl;
	int** tablaUniverso = cand->generateTable();

	// "precedenceR" es un arreglo de objetos (PrecedenceRestrictions) que vamos a usar para extraer las ecuaciones booleanas
	// de las restricciones que si son validas, es decir que generan una restriccion.
	vector<PrecedenceRestriction *> arrPrecedence = model->getPrecedenceRestrictions();

	// Generar restricciones de precedencia negadas
	/*
	* 1. Un metodo que extraiga de la restriccion la combiancion de letras que no se puede
	* 2. Determinar la negacion de esta expression
	* 3. Crear un arreglo de string
	* 4. Guardar las restricciones generadas en este arreglo de strings
	*/

	/*
	* La idea va a ser recorrer la tabla del universo de variables en la columna de origen hacia abajo y para cada uno de los renglones
	* Verificar que no exista un valor possible anterior en estacion para el actual.
	*

	*/

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//PRECEDENCE CONSTRAINT GENERATION
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << "\n\n ------RESTRICCIONES PRECEDENCIA BOOLEANAS----- \n\n" << endl;
	//formato correcto
	//Esto puede ser una funcion fuera de MAIN
	vector<string> rPrecedence;

	int origin, staOrigin;
	int dest, staDestination;
	int j = numberStations - 1;
	int jDest = 0;
	bool flag = false;

	for (int i = 0; i<arrPrecedence.size(); i++) {

		origin = arrPrecedence.at(i)->getOrigin();
		dest = arrPrecedence.at(i)->getDestination();

		cout << "ERROR EN PRECEDENCIA" << endl;
		cout << "Origen: " << origin << endl;
		cout << "Destino: " << dest << endl;

		while (j>0) {
			//[estacion][actividad]
			if (tablaUniverso[j][origin] == 1) {
				staOrigin = j;
				flag = true;
			}


			while (flag && jDest<numberStations) {
				//[estacion][actividad]
				if (tablaUniverso[jDest][dest] == 1) {
					if (j>jDest) {
						cout << "j: " << j << endl;
						cout << "JDest: " << jDest << endl;

						staDestination = jDest;
						string temp = "";
						string temp1 = getNegation(origin, staOrigin, tablaUniverso, numberStations);
						cout << "NEGACION ORIGIN: " << temp1 << endl;
						string temp2 = getNegation(dest, staDestination, tablaUniverso, numberStations);
						cout << "NEGACION DEST: " << temp2 << endl;

						temp += temp1;
						temp += ",";
						temp += temp2;
						cout << temp << endl;
						// agregar a arreglo de restricciones de Precedencia
						rPrecedence.push_back(temp);

					}
				}
				jDest++;
			}
			jDest = 0;
			j--;
			flag = false;

		}
		j = numberStations - 1;

	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//UNICITY CONSTRAINT GENERATION
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << "\n\n ------RESTRICTIONS UNICITY BOOLEANAS----- \n\n" << endl;

	vector<string> rUnicity = model->getUnicityRestrictions();

	string toString;
	int stationUnicity;
	for (int ii = 0; ii< cand->getNumberActivities(); ii++) {
		if (cand->getCandStationsForActivity(ii)->hasOnlyOneCandidate()) {
			string UnicityConstr;
			toString = invHashAux(ii);
			UnicityConstr.append(toString);
			UnicityConstr.append(cand->getCandStationsForActivity(ii)->getCandidatesExtra());
			rUnicity.push_back(UnicityConstr);

		}
	}

	for (int i = 0; i< rUnicity.size(); i++) {
		cout << rUnicity.at(i) << endl;
	}

	// cand tiene el numero de actividades de ahi puedo generar la ultima restriccion
	// number of stations numberStations


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//DURATION CONSTRAINT GENERATION
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << "\n\n ------RESTRICTIONS DURACION BOOLEANAS----- \n" << endl;

	int numRecDuration = model->getNumberEquations();
	vector<Equation *> equations = model->getVectorEquations();
	vector<vector<DurationNode>> groupDN;

	for (int w = 0; w<numRecDuration; w++) {
		vector<DurationNode> arr;
		int y = 0;
		Variable * aux;
		int balanceDuration = heuristic - equations[w]->getConstantValue();

		while (y<equations[w]->getNumVariables()) {
			aux = equations[w]->getVariable(y);
			DurationNode * DN = new DurationNode(aux->getDuration(), aux->getActSta(), balanceDuration);
			//Add Duration Node to Vector
			arr.push_back(*DN);
			y = y + 1;
		}

		//Add Vector of Duration Nodes to array of restrictions.
		groupDN.push_back(arr);
	}

	// Verificar la creacion de los nodos de duracion.
	int size = groupDN.size();
	vector<DurationNode> arr;

	for (int i = 0; i<size; i++) {
		arr = groupDN[i];

		//esta vacio
		if (arr.size() != 0) {
			groupDN[i] = MergeSort(arr);
			int numDNs = groupDN[i].size();
			//cout << "\n \n";

			for (int j = 0; j<numDNs; j++) {
				groupDN[i][j].setId(j);
				// Imprimir los nodos ordenados.
				/*
				cout << "Nodo " << j << endl;
				cout<< "Duracion: " << groupDN[i][j].getDuration() << endl;
				cout<< "Restriction: " << groupDN[i][j].getRestriction() << endl;
				cout<< "Id: " << groupDN[i][j].getId() <<endl;
				cout<< "BalanceDuration: " << groupDN[i][j].getBalanceDuration() <<endl;
				cout << "\n";
				*/
			}
		}

	}


	vector<string> vectorRest;
	vector<string> * vectorR = &vectorRest;

	cout << groupDN.size() << endl;
	for (int j = 0; j<groupDN.size(); j++) {
		vector<DurationNode> * vectorDN = &groupDN[j];
		if (vectorDN->size() != 0) {
			DurationTree * tree = new DurationTree(vectorDN, vectorR, groupDN[j][0].getBalanceDuration());
			tree->buildTree();
		}


	}

	// NUMBER OF DURATION RESTRICTIONS GENERATED
	cout << "Restricciones Generadas:" << vectorR->size() << endl;


	//for(int i = 0; i < vectorR->size() ; i++){
	//    //cout << vectorR->at(i) << endl;
	//}

	/*
	cout << "Prueba de negacion de una restriccion: " << endl;

	string negC = constraintNeg("a0", tablaUniverso, numberStations);

	cout << "Negacion: " << negC << endl;

	cout << cand->getCandStationsForActivity(9)->getCandidatesExtra() << endl;
	//cand->getCandStationsForActivity(0)->hasOnlyOneCandidate()
	* */


	cout << "Negacion de Restricciones Generadas:" << vectorR->size() << endl;

	for (int i = 0; i < vectorR->size(); i++) {
		string negC = constraintNeg(vectorR->at(i), tablaUniverso, numberStations);
		vectorR->at(i) = negC;
		//cout << vectorR->at(i) << endl;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//CONSTRAINT BINARY INTEGRATION IN PARALLEL
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//PARALLEL CALL FUNCTION: runtest()


	cout << "Integracion Booleana" << endl;
	//BinaryMixer bMixer;
	//Mixer * mixer = new Mixer();

	//////////////////////////////
	/////Variable Declaration/////
	//////////////////////////////
	//const int width = 10;
	//const unsigned long long int height = 4000000000;
	//int** ptr6 = new int*[width];
	//for (int i = 0; i < width; ++i)
	//	ptr6[i] = new int[height];

	//unsigned int * ptr7 = new unsigned int[10];
	//memset(ptr7, 0, 10 * sizeof(int));

	//int** ptr8 = new int*[width];
	//for (int i = 0; i < width; ++i)
	//	ptr8[i] = new int[height];

	//unsigned int * ptr9 = new unsigned int[10];
	//memset(ptr9, 0, 10 * sizeof(int));


	const unsigned long long int sol_vector_size = 4294967295;
	const unsigned long long int rest_vector_size = 10000;

	//Auxiliar int arrays
	//unsigned int S1[1000];
	//unsigned int S2[75000];
	unsigned int ARR1[rest_vector_size]; //225,000 -> 56,250 restricciones de 4 estaciones
										 //unsigned int no_dup_sol[75000];
										 //Pointers to int arrays
	unsigned int * ptr1 = new unsigned int[rest_vector_size];
	unsigned int * ptr2 = new unsigned int[sol_vector_size];
	unsigned int * ptr3 = ARR1;
	unsigned int * ptr4 = new unsigned int[sol_vector_size];

	//inicializar en ceros
	memset(ptr1, 0, rest_vector_size * sizeof(int));
	memset(ptr3, 0, rest_vector_size * sizeof(int));
	memset(ptr2, 0, sol_vector_size * sizeof(int));
	memset(ptr4, 0, sol_vector_size * sizeof(int));

	//In order to keep track how many elements there are in certain array
	//Number of occupied space in the array
	int S1_size = 0;
	int S2_size = 0;
	int ARR1_size = 0;
	unsigned long long int NoDup_size = 0;

	//AUXILIAR Counters
	unsigned long long int  jj = 0;
	unsigned long long int solution_cont = 0;
	unsigned long long int cont_GPU = 0;
	unsigned long long int * ptr_cont;
	ptr_cont = &cont_GPU;

	//HASH TABLE
	//string hash_key;
	//string null_hash_key;
	//for (int i = 0; i < numberStations; i++) {
	//	null_hash_key.append("0");
	//}
	//std::map<string, bool> mapTable;
	//mapTable[null_hash_key] = true;


	/////////////////////////////////////////////////////
	////////////////HASHMAP//////////////////////////////
	/////////////////////////////////////////////////////
	const unsigned long long int size_hashmap = (2147483647);
	unsigned long long int hash_key = 0;
	unsigned long long int null_hash_key = 0;
	unsigned long long int control_location = 1;

	//CUKOO HASHING MULITPLE HASH MAPS
	bool * ptr_hashmap = new bool[size_hashmap];
	bool * ptr1_hashmap = new bool[size_hashmap];
	bool * ptr2_hashmap = new bool[size_hashmap];

	memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
	memset(ptr1_hashmap, false, sizeof(bool) * size_hashmap);
	memset(ptr2_hashmap, false, sizeof(bool) * size_hashmap);

	//AUX VARIABLES
	const unsigned long long int seed = 16777619;
	const unsigned long long int seed1 = 2166136261;
	const unsigned long long int seed2 = 142010;
	unsigned int h = seed;
	unsigned int h1 = seed1;
	unsigned int h2 = seed2;
	const unsigned long long int prime = 16777619;
	const unsigned int m = 0x5bd1e995;
	const int hash_r = 24;
	unsigned int k = 0;


	//TIME
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	auto duration = 0;

	std::chrono::high_resolution_clock::time_point t3;
	std::chrono::high_resolution_clock::time_point t4;
	auto duration2 = 0;

	// S1: Array that will store the temporal solutions.
	// S2: Array that will store the temporal solution
	// ARR1: Array that will store the new constraint that will be merged with the temporal solution.

	// Transform and save 1st constrain in S1.
	cout << vectorR->at(0) << endl;
	S1_size = StrRestr2BinVector(numberStations, vectorR->at(0), ptr1);
	cout << "Size Temp:  \n" << S1_size / numberStations << endl;

	printArrayPointer(ptr1, S1_size, numberStations);

	// Transform and save 2nd constrain in ARR1.
	cout << rPrecedence.at(1) << endl;
	ARR1_size = StrRestr2BinVector(numberStations, rPrecedence.at(1), ptr3);
	cout << "Size Restriction: \n" << ARR1_size / numberStations << endl;

	printArrayPointer(ptr3, ARR1_size, numberStations);

	//Inicia Tiempo 
	t1 = std::chrono::high_resolution_clock::now();

	//////////////////////////////////////
	/////EXECUTE PARALLEL INTEGRATION/////
	//////////////////////////////////////

	runTest(argc, (const char **)argv, ptr_cont, numberStations, ptr1, S1_size, ptr3, ARR1_size, ptr2);
	
	printArrayPointer(ptr2, (S1_size*ARR1_size) / numberStations, numberStations);



	solution_cont = 0;

	// Initialize the hash to a 'random' value
	memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
	//memset(ptr1_hashmap, false, sizeof(bool) * size_hashmap);
	memset(ptr2_hashmap, false, sizeof(bool) * size_hashmap);

	while (jj < (*ptr_cont)*numberStations) {
		h = seed;
		h1 = seed1;
		h2 = seed2;
		for (int kk = 0; kk < numberStations; kk++) {
			h = MurmurHash2_pt1((*(ptr2 + jj + kk)),h);
			h1 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h1);
			h2 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h2);

		}

		h = MurmurHash2_pt2(h);
		h1 = MurmurHash2_pt2(h1);
		h2 = MurmurHash2_pt2(h2);
		
		h = h % size_hashmap;
		h1 = h1 % size_hashmap;
		h2 = h2 % size_hashmap;
		
		if (!*(ptr_hashmap + h) && !*(ptr1_hashmap + h1) && !*(ptr2_hashmap + h2)) {

			*(ptr_hashmap + h) = true;
			*(ptr1_hashmap + h1) = true;
			*(ptr2_hashmap + h2) = true;

			for (int kk = 0; kk < numberStations; kk++) {
				*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
			}
			solution_cont += numberStations;


		}
		else {
			if (*(ptr_hashmap + h) && *(ptr1_hashmap + h1) && *(ptr2_hashmap + h2)) {
			}
			else {
				*(ptr_hashmap + h) = true;
				*(ptr1_hashmap + h1) = true;
				*(ptr2_hashmap + h2) = true;

				for (int kk = 0; kk < numberStations; kk++) {
					*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
				}
				solution_cont += numberStations;
			}
		}

		jj += numberStations;
		//hash_key = 0;
		//control_location = 1;
		

	}
	NoDup_size = solution_cont;


	cout << "Tamano TEMP: " << NoDup_size << endl;

	printArrayPointer(ptr4, NoDup_size, numberStations);
	//cout << "Number of solutions: " << NoDup_size << endl;

	//SUMA DE RESTRICCIONES NO ME ESTA DANDO EXACTA???
	int contador = 1;
	int totalRestrictions = vectorR->size() + rPrecedence.size() + rUnicity.size() - 1;

	cout << "size P: " << rPrecedence.size() << endl;
	cout << "size D: " << vectorR->size() << endl;
	cout << "size U: " << rUnicity.size() << endl;

	///////////////////////////////////////////
	/////PRECEDENT RESTRICTION INTEGRATION/////
	///////////////////////////////////////////

	int cont_mix = 1;

	//for(int i = 0; i < rPrecedence.size() ; i++){
	while (cont_mix < rPrecedence.size() && NoDup_size>0) {
		//Reset values of ARR1 to zero.
		memset(ARR1, 0, sizeof(ARR1));

		*ptr_cont = 0;
		//Transform String::Constraint into Array[int]::Constraint
		ARR1_size = StrRestr2BinVector(numberStations, rPrecedence.at(cont_mix), ptr3);
		cout << "-----------------------------------------------------" << endl;
		cout << "RESTRICCION" << endl;
		//printArrayPointer(ptr3, ARR1_size, numberStations);
		cout << "-----------------------------------------------------" << endl;
		//Parallel Execution
		//cout << "elements en temp: " << NoDup_size << " \t Elements in REST: " << ARR1_size << endl;
		//cout << "Elements in solutions vector: " << (NoDup_size*ARR1_size) / numberStations;


		cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
		t3 = std::chrono::high_resolution_clock::now();

		runTest(argc, (const char **)argv, ptr_cont, numberStations, ptr4, solution_cont, ptr3, ARR1_size, ptr2);

		t4 = std::chrono::high_resolution_clock::now();
		duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
		cout << "Tiempo: " << duration2 << endl;

		solution_cont = 0;
		jj = 0;
		*(ptr_hashmap + null_hash_key) = 0;
		cout << "elementos para hash: " << *ptr_cont << endl;
		memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
		solution_cont = 0;

		// Initialize the hash to a 'random' value
		memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
		//memset(ptr1_hashmap, false, sizeof(bool) * size_hashmap);
		memset(ptr2_hashmap, false, sizeof(bool) * size_hashmap);

		while (jj < (*ptr_cont)*numberStations) {
			h = seed;
			h1 = seed1;
			h2 = seed2;
			for (int kk = 0; kk < numberStations; kk++) {
				h = MurmurHash2_pt1((*(ptr2 + jj + kk)), h);
				h1 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h1);
				h2 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h2);

			}

			h = MurmurHash2_pt2(h);
			h1 = MurmurHash2_pt2(h1);
			h2 = MurmurHash2_pt2(h2);

			h = h % size_hashmap;
			h1 = h1 % size_hashmap;
			h2 = h2 % size_hashmap;

			if (!*(ptr_hashmap + h) && !*(ptr1_hashmap + h1) && !*(ptr2_hashmap + h2)) {

				*(ptr_hashmap + h) = true;
				*(ptr1_hashmap + h1) = true;
				*(ptr2_hashmap + h2) = true;

				for (int kk = 0; kk < numberStations; kk++) {
					*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
				}
				solution_cont += numberStations;


			}
			else {
				if (*(ptr_hashmap + h) && *(ptr1_hashmap + h1) && *(ptr2_hashmap + h2)) {
				}
				else {
					*(ptr_hashmap + h) = true;
					*(ptr1_hashmap + h1) = true;
					*(ptr2_hashmap + h2) = true;

					for (int kk = 0; kk < numberStations; kk++) {
						*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
					}
					solution_cont += numberStations;
				}
			}

			jj += numberStations;
			//hash_key = 0;
			//control_location = 1;


		}

		NoDup_size = solution_cont;
		cout << "-----------------------------------------------------" << endl;
		cout << "SOLUTION" << endl;
		cout << "Number of solutions: " << NoDup_size / numberStations << endl;
		//printArrayPointer(ptr4, (NoDup_size), numberStations);

		cout << "-----------------------------------------------------" << endl;
		contador++;
		cout << "Iteracion " << contador << "/" << totalRestrictions << endl;

		cont_mix++;
	}


	///////////////////////////////////////////////
	/////////DURATION RESTRICTION INTEGRATION/////
	///////////////////////////////////////////////

	cont_mix = 2;

	//for (int i = 2; i < vectorR->size(); i++) {
	while (cont_mix < vectorR->size() && NoDup_size>0) {
		//Transform String::Constraint into Array[int]::Constraint
		//Reset values of ARR1 to zero.
		memset(ARR1, 0, sizeof(ARR1));
		*ptr_cont = 0;
		ARR1_size = StrRestr2BinVector(numberStations, vectorR->at(cont_mix), ptr3);
		cout << "-----------------------------------------------------" << endl;
		cout << "RESTRICCION" << endl;
		//printArrayPointer(ptr3, ARR1_size, numberStations);
		cout << "-----------------------------------------------------" << endl;
		//Parallel Execution
		//cout << "elements en temp: " << NoDup_size << " \t Elements in REST: " << ARR1_size << endl;


		cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
		t3 = std::chrono::high_resolution_clock::now();

		runTest(argc, (const char **)argv, ptr_cont, numberStations, ptr4, solution_cont, ptr3, ARR1_size, ptr2);

		t4 = std::chrono::high_resolution_clock::now();
		duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
		cout << "Tiempo: " << duration2 << endl;





		solution_cont = 0;
		jj = 0;
		*(ptr_hashmap + null_hash_key) = 0;
		cout << "elementos para hash: " << *ptr_cont << endl;
		memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
		solution_cont = 0;
		// Initialize the hash to a 'random' value
		memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
		memset(ptr1_hashmap, false, sizeof(bool) * size_hashmap);
		memset(ptr2_hashmap, false, sizeof(bool) * size_hashmap);

		while (jj < (*ptr_cont)*numberStations) {
			h = seed;
			h1 = seed1;
			h2 = seed2;
			for (int kk = 0; kk < numberStations; kk++) {
				h = MurmurHash2_pt1((*(ptr2 + jj + kk)), h);
				h1 = fnv1((*(ptr2 + jj + kk)), h1);
				h2 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h2);

			}

			h = MurmurHash2_pt2(h);
			//h1 = MurmurHash2_pt2(h1);
			h2 = MurmurHash2_pt2(h2);

			h = h % size_hashmap;
			h1 = h1 % size_hashmap;
			h2 = h2 % size_hashmap;

			if (!*(ptr_hashmap + h) && !*(ptr1_hashmap + h1) && !*(ptr2_hashmap + h2)) {

				*(ptr_hashmap + h) = true;
				*(ptr1_hashmap + h1) = true;
				*(ptr2_hashmap + h2) = true;

				for (int kk = 0; kk < numberStations; kk++) {
					*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
				}
				solution_cont += numberStations;


			}
			else {
				if (*(ptr_hashmap + h) && *(ptr1_hashmap + h1) && *(ptr2_hashmap + h2)) {
				}
				else {
					*(ptr_hashmap + h) = true;
					*(ptr1_hashmap + h1) = true;
					*(ptr2_hashmap + h2) = true;

					for (int kk = 0; kk < numberStations; kk++) {
						*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
					}
					solution_cont += numberStations;
				}
			}

			jj += numberStations;
			//hash_key = 0;
			//control_location = 1;


		}
		NoDup_size = solution_cont;
		cout << "-----------------------------------------------------" << endl;
		cout << "SOLUTION" << endl;
		cout << "Number of solutions: " << NoDup_size / numberStations << endl;
		//printArrayPointer(ptr4, (NoDup_size), numberStations);
		cout << "-----------------------------------------------------" << endl;
		contador++;
		cout << "Iteracion " << contador << "/" << totalRestrictions << endl;

		cont_mix++;
	}


		///////////////////////////////////////////
		/////PRECEDENT RESTRICTION INTEGRATION/////
		///////////////////////////////////////////

	cont_mix = 0;
	//for(int i = 0; i < rPrecedence.size() ; i++){
	while (cont_mix < rPrecedence.size() && NoDup_size>0) {
		//Reset values of ARR1 to zero.
		memset(ARR1, 0, sizeof(ARR1));

		*ptr_cont = 0;
		//Transform String::Constraint into Array[int]::Constraint
		ARR1_size = StrRestr2BinVector(numberStations, rPrecedence.at(cont_mix), ptr3);
		cout << "-----------------------------------------------------" << endl;
		cout << "RESTRICCION" << endl;
		//printArrayPointer(ptr3, ARR1_size, numberStations);
		cout << "-----------------------------------------------------" << endl;
		//Parallel Execution
		//cout << "elements en temp: " << NoDup_size << " \t Elements in REST: " << ARR1_size << endl;
		//cout << "Elements in solutions vector: " << (NoDup_size*ARR1_size) / numberStations;


		cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
		t3 = std::chrono::high_resolution_clock::now();

		runTest(argc, (const char **)argv, ptr_cont, numberStations, ptr4, solution_cont, ptr3, ARR1_size, ptr2);

		t4 = std::chrono::high_resolution_clock::now();
		duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
		cout << "Tiempo: " << duration2 << endl;

		solution_cont = 0;
		jj = 0;
		*(ptr_hashmap + null_hash_key) = 0;
		cout << "elementos para hash: " << *ptr_cont << endl;
		memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
		solution_cont = 0;

		// Initialize the hash to a 'random' value
		memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
		//memset(ptr1_hashmap, false, sizeof(bool) * size_hashmap);
		memset(ptr2_hashmap, false, sizeof(bool) * size_hashmap);

		while (jj < (*ptr_cont)*numberStations) {
			h = seed;
			h1 = seed1;
			h2 = seed2;
			for (int kk = 0; kk < numberStations; kk++) {
				h = MurmurHash2_pt1((*(ptr2 + jj + kk)), h);
				h1 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h1);
				h2 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h2);

			}

			h = MurmurHash2_pt2(h);
			h1 = MurmurHash2_pt2(h1);
			h2 = MurmurHash2_pt2(h2);

			h = h % size_hashmap;
			h1 = h1 % size_hashmap;
			h2 = h2 % size_hashmap;

			if (!*(ptr_hashmap + h) && !*(ptr1_hashmap + h1) && !*(ptr2_hashmap + h2)) {

				*(ptr_hashmap + h) = true;
				*(ptr1_hashmap + h1) = true;
				*(ptr2_hashmap + h2) = true;

				for (int kk = 0; kk < numberStations; kk++) {
					*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
				}
				solution_cont += numberStations;


			}
			else {
				if (*(ptr_hashmap + h) && *(ptr1_hashmap + h1) && *(ptr2_hashmap + h2)) {
				}
				else {
					*(ptr_hashmap + h) = true;
					*(ptr1_hashmap + h1) = true;
					*(ptr2_hashmap + h2) = true;

					for (int kk = 0; kk < numberStations; kk++) {
						*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
					}
					solution_cont += numberStations;
				}
			}

			jj += numberStations;
			//hash_key = 0;
			//control_location = 1;


		}

		NoDup_size = solution_cont;
		cout << "-----------------------------------------------------" << endl;
		cout << "SOLUTION" << endl;
		cout << "Number of solutions: " << NoDup_size / numberStations << endl;
		//printArrayPointer(ptr4, (NoDup_size), numberStations);

		cout << "-----------------------------------------------------" << endl;
		contador++;
		cout << "Iteracion " << contador << "/" << totalRestrictions << endl;

		cont_mix++;
	}




	////	/////////////////////////////////////////
	////	/////UNICITY RESTRICTION INTEGRATION/////
	////	/////////////////////////////////////////

		cont_mix =  0;

		//for (int i = 0; i < rUnicity.size(); i++) {
		while(cont_mix < rUnicity.size() && NoDup_size>0){
			//Transform String::Constraint into Array[int]::Constraint
			//Reset values of ARR1 to zero.
			memset(ARR1, 0, sizeof(ARR1));

			*ptr_cont = 0;
			ARR1_size = StrRestr2BinVector(numberStations, rUnicity.at(cont_mix), ptr3);
			cout << "-----------------------------------------------------" << endl;
			cout << "RESTRICCION" << endl;
			//printArrayPointer(ptr3, ARR1_size, numberStations);
			cout << "-----------------------------------------------------" << endl;
			//Parallel Execution
			//cout << "elements en temp: " << NoDup_size << " \t Elements in REST: " << ARR1_size << endl;
			cout << "Iteracion " << contador << "/" << totalRestrictions << endl;
			t3 = std::chrono::high_resolution_clock::now();

			runTest(argc, (const char **)argv, ptr_cont, numberStations, ptr4, solution_cont, ptr3, ARR1_size, ptr2);

			t4 = std::chrono::high_resolution_clock::now();
			duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
			cout << "Tiempo: " << duration2 << endl;

			solution_cont = 0;
			jj = 0;
			*(ptr_hashmap + null_hash_key) = 0;
			cout << "elementos para hash: " << *ptr_cont << endl;
			memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
			solution_cont = 0;

			// Initialize the hash to a 'random' value
			memset(ptr_hashmap, false, sizeof(bool) * size_hashmap);
			memset(ptr1_hashmap, false, sizeof(bool) * size_hashmap);
			memset(ptr2_hashmap, false, sizeof(bool) * size_hashmap);

			while (jj < (*ptr_cont)*numberStations) {
				h = seed;
				h1 = seed1;
				h2 = seed2;
				for (int kk = 0; kk < numberStations; kk++) {
					h = MurmurHash2_pt1((*(ptr2 + jj + kk)), h);
					h1 = fnv1((*(ptr2 + jj + kk)), h1);
					h2 = MurmurHash2_pt1((*(ptr2 + jj + kk)), h2);

				}

				h = MurmurHash2_pt2(h);
				//h1 = MurmurHash2_pt2(h1);
				h2 = MurmurHash2_pt2(h2);

				h = h % size_hashmap;
				h1 = h1 % size_hashmap;
				h2 = h2 % size_hashmap;

				if (!*(ptr_hashmap + h) && !*(ptr1_hashmap + h1) && !*(ptr2_hashmap + h2)) {

					*(ptr_hashmap + h) = true;
					*(ptr1_hashmap + h1) = true;
					*(ptr2_hashmap + h2) = true;

					for (int kk = 0; kk < numberStations; kk++) {
						*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
					}
					solution_cont += numberStations;


				}
				else {
					if (*(ptr_hashmap + h) && *(ptr1_hashmap + h1) && *(ptr2_hashmap + h2)) {
					}
					else {
						*(ptr_hashmap + h) = true;
						*(ptr1_hashmap + h1) = true;
						*(ptr2_hashmap + h2) = true;

						for (int kk = 0; kk < numberStations; kk++) {
							*(ptr4 + solution_cont + kk) = *(ptr2 + jj + kk);
						}
						solution_cont += numberStations;
					}
				}

				jj += numberStations;
				//hash_key = 0;
				//control_location = 1;

			}
			NoDup_size = solution_cont;
			cout << "-----------------------------------------------------" << endl;
			cout << "SOLUTION" << endl;
			cout << "Number of solutions: " << NoDup_size/numberStations << endl;
			//printArrayPointer(ptr4, (NoDup_size), numberStations);
			cout << "-----------------------------------------------------" << endl;
			contador++;
			cout << "Iteracion " << contador << "/" << totalRestrictions << endl;

			cont_mix++;
		}



	//If que controle cuando acabo
	if ((NoDup_size / numberStations) != 0) {

		//Tiempo Fin
		t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		cout << "Tiempo: " << duration << endl;

		finish = true;

		cout << "-----------------------------------------------------" << endl;
		cout << "SOLUTION" << endl;
		cout << "Tiempo: " << duration << endl;
		cout << "Number of constraints:  " <<  totalRestrictions << endl;
		cout << "Number of solutions: " << NoDup_size / numberStations << endl;
		cout << "\nBalancing Time: " << heuristic << "\n\n" << endl;
		printArrayPointer(ptr4, (NoDup_size), numberStations);
		cout << "-----------------------------------------------------" << endl;


	}
	else {
		heuristic = heuristic + gdc;
	}

	  } //if cand is feasible
	  else
	  {
	      cout << "No hay solucion." << endl;
	heuristic = heuristic + gdc;
	  }
	  
	  } // iteracion de tiempo de balanceo while



	return true;

}



int main(int argc, const char **argv) {

	cout << "Hello world\n";

    //bool flag = pruebas(argc, (const char **)argv);
	//bool flag = parallelExeHash(argc, (const char **)argv);
	//bool flag = parallelExe(argc, (const char **)argv);


	bool flag = executeBool();
    
    //cout << "Termino: " << flag << endl;

	//int * ptr;
	//ptr = StrRestr2BinVector(4, "a0:b0:c0:d0:ad0");

	//cout << *(ptr + 0) << endl;
	//printArrayPointer(ptr, 8, numberStations);


	//int* hashtable = new int[1073741824];
	//cout << hashiii(1111) << endl;
	////hashtable[hashiii(1111)] = 0;
	//cout << hashtable[hashiii(1111)] << endl;




	cin.get();

	return 0;

}


    



