/*
* File:   VectorBR.cpp
* Author: normalazo
*
* Created on November 22, 2017, 6:42 PM
*/

#include "VectorBR.h"

VectorBR::VectorBR(int numStations, string restr) {

	vector<string> vectorsR;
	Tokenize(restr, vectorsR, ",");                           //Separo el VECTOR por los operadores OR
	for (int w = 0; w<vectorsR.size(); w++) {
		BinaryRestriction aux(numStations, vectorsR.at(w));
		this->Br.push_back(aux);
	}

}