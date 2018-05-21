/*
* File:   BinaryRestriction.h
* Author: normalazo
*
* Created on November 16, 2017, 6:36 PM
*/

#ifndef BINARYRESTRICTION_H
#define	BINARYRESTRICTION_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <bitset>
#include "Utilities.h"


class BinaryRestriction {
public:



	bitset<32> Table[10];
	//Number of stations
	int numStations;

	BinaryRestriction(int, string);
	BinaryRestriction(int);
	bool valid();
	void print();
	string HashKey();


private:

};

#endif	/* BINARYRESTRICTION_H */