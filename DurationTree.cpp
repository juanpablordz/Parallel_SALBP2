/* 
 * File:   DurationTree.cpp
 * Author: normalazo
 * 
 * Created on August 10, 2017, 10:59 PM
 */

#include "DurationTree.h"

    // Constructor
    DurationTree::DurationTree(vector<DurationNode>* vectorDN, vector<string>* vectorR, int balanceDuration){
        this->root = new DurationNode(-1, 0, balanceDuration);
        this->vectorR = vectorR;
        this->vectorDN = vectorDN;
        this->balanceDuration = balanceDuration;
       
    }
    

    void DurationTree::buildTree(){
        this->root->expand(this->vectorR, this->vectorDN);
    }
    
    DurationNode* DurationTree::getRoot(){
        return this->root;
    }
    