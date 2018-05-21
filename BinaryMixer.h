/*
* File:   BinaryMixer.h
* Author: normalazo
*
* Created on November 22, 2017, 7:18 PM
*/

#ifndef BINARYMIXER_H
#define	BINARYMIXER_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <bitset>
#include "Utilities.h"
#include "BinaryRestriction.h"
#include "VectorBR.h"
#include <map>

class BinaryMixer {
public:
	BinaryMixer();
	vector<BinaryRestriction> mix(vector<BinaryRestriction>, vector<BinaryRestriction>);
	BinaryRestriction OR(BinaryRestriction, BinaryRestriction);
private:

};

#endif	/* BINARYMIXER_H */