/* 
 * File:   NegRestriction.cpp
 * Author: normalazo
 * 
 * Created on July 18, 2017, 8:28 PM
 */

#include <iostream>
#include "NegRestriction.h"
#include "Utilities.h"

using namespace std;

NegRestriction::NegRestriction(int activity, int station, int** tableUniverse) {
    NegRestriction::activity = activity;
    NegRestriction::station = station;
    NegRestriction::tableUniverse = tableUniverse;
    
}

string NegRestriction::getNegation(){
    bool flag= true;
    string str, activityLetter;
    int activityNum = activity;
    activityLetter = invHashAux(activity);
    int i = 0;
    
    while(tableUniverse[i]!= NULL){
        if(i != station && tableUniverse[i][activity] == 1){
            if(!flag){
                str.append(";");
                flag=false;
            }
            str.append(activityLetter);
            //???
            str.append(to_string(i)); //como se comvertia un int en su numero para hacer append a un string
        }
        i++;
    }
    
    return str;
    
}