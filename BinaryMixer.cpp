/* 
 * File:   BinaryMixer.cpp
 * Author: normalazo
 * 
 * Created on November 22, 2017, 7:18 PM
 */

#include "BinaryMixer.h"

BinaryMixer::BinaryMixer() {
}

vector<BinaryRestriction> BinaryMixer::mix(vector<BinaryRestriction> temp, vector<BinaryRestriction> restriction) {
	std::map<string, bool> mapTable;

	vector<BinaryRestriction> sol;
	//inicializar hash

	//recorrer la temp
	for (int i = 0; i<temp.size(); i++) {

		//el problema esta aqui con el manejo de las contadores
		for (int j = 0; j<restriction.size(); j++) {
			BinaryRestriction aux(temp[i].numStations);
			aux = OR(temp[i], restriction[j]);
			//falta verificar si ya existe
			if (aux.valid() && !mapTable[aux.HashKey()]) {
				mapTable[aux.HashKey()] = true;
				sol.push_back(aux);
			}

		}


	}



	return sol;

}

BinaryRestriction BinaryMixer::OR(BinaryRestriction a, BinaryRestriction b) {
	BinaryRestriction sol(a.numStations);

	for (int i = 0; i<a.numStations; i++) {
		sol.Table[i] = a.Table[i] | b.Table[i];
	}

	return sol;
}

