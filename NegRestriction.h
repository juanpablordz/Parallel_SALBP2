/* 
 * File:   NegRestriction.h
 * Author: normalazo
 *
 * Created on July 18, 2017, 8:28 PM
 */

#ifndef NEGRESTRICTION_H
#define	NEGRESTRICTION_H

#include <iostream>

using namespace std;

class NegRestriction 
{
    
public:
    NegRestriction(int activity, int station, int** tableUniverse);
    string getNegation();
    
private:
    int station;
    int activity;
    int** tableUniverse;


};

#endif	/* NEGRESTRICTION_H */

