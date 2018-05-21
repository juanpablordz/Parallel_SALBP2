/*
* File:   VectorBR.h
* Author: normalazo
*
* Created on November 22, 2017, 6:42 PM
*/

#ifndef VECTORBR_H
#define	VECTORBR_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <bitset>
#include "Utilities.h"
#include "BinaryRestriction.h"


class VectorBR {
public:
	VectorBR(int, string);
	vector<BinaryRestriction> Br;
private:

};

#endif	/* VECTORBR_H */