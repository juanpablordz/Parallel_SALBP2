/* 
 * File:   BinaryRestriction.cpp
 * Author: normalazo
 * 
 * Created on November 16, 2017, 6:36 PM
 */

#include "BinaryRestriction.h"

BinaryRestriction::BinaryRestriction(int numStations) {
	this->numStations = numStations;

}

BinaryRestriction::BinaryRestriction(int numStations, string restr) {

	//Creo tabla para manejar la restriccion tamaño [numStarions x 32]
	this->numStations = numStations;
	int numActR, numStaR;
	vector<string> elementsR;
	Tokenize(restr, elementsR, ":");               //Separo vectores de solucion en cada variable binaria

	for (int k = 0; k<elementsR.size(); k++) {
		numActR = hashAux(actLetter(elementsR[k]));
		numStaR = numStation(elementsR[k]);
		this->Table[numStaR][31 - numActR] = 1;
	}



}

//BinaryRestriction BinaryRestriction::OR(BinaryRestriction a, BinaryRestriction b) {
//    
//}

bool BinaryRestriction::valid() {

	bitset<32> cum;
	bitset<32> aux;
	bool flag = false;

	int i = 0;
	while (i<this->numStations && !flag) {
		aux = cum&Table[i];
		flag = aux.any();
		cum = cum^Table[i];
		i++;
	}

	return !flag;

}

void BinaryRestriction::print() {


	for (int i = 0; i<this->numStations; i++) {
		cout << this->Table[i] << endl;
	}


}

string BinaryRestriction::HashKey() {
	string aux;
	for (int i = 0; i<this->numStations; i++) {
		aux.append(std::to_string(Table[i].to_ulong()));
	}

	return aux;



}



