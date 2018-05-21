/* 
 * File:   DurationTree.h
 * Author: normalazo
 *
 * Created on August 10, 2017, 10:59 PM
 */

#ifndef DURATIONTREE_H
#define	DURATIONTREE_H


#include <iostream>
#include "DurationNode.h"
#include <string>
#include <vector>

using namespace std;

class DurationTree {
public:
    
    //Revisar cmbio constructor
    
     
    // Constructor
    DurationTree(vector<DurationNode>* vectorDN, vector<string>* vectorR, int balanceDuration);
    
    //Getters
    DurationNode* getRoot();
    
    
    // 1. Sort vector of DurationNodes
    void buildTree();
    

private:
    
    vector<DurationNode>* vectorDN;
    vector<string>* vectorR;
    int balanceDuration;
    DurationNode* root;

    

    
    

};

#endif	/* DURATIONTREE_H */

