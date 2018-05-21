/* 
 * File:   DurationNode.cpp
 * Author: normalazo
 * 
 * Created on August 10, 2017, 10:59 PM
 */

#include "DurationNode.h"


//this also has to create the string
DurationNode::DurationNode(int id, int duration, int balanceDuration) {
    
    this->duration = duration;
    this->id = id;
    this->balanceDuration = balanceDuration;
    
    
}

DurationNode::DurationNode(int id, int duration) {
    
    this->duration = duration;
    this->id = id;
    
}

DurationNode::DurationNode(int duration, string restriction, int balanceDuration) {
    
    this->id = 0;
    this->duration = duration;
    this->restriction = restriction;
    this->balanceDuration = balanceDuration;
    
    
    
}

DurationNode::DurationNode() {

}


//GET methods
DurationNode DurationNode::getChild(int i){
    return this->children.at(i);
}

int DurationNode::getId(){ 
    return this->id;  
}

int DurationNode::getDuration(){
    return this->duration;
}

int DurationNode::getBalanceDuration(){
    return this->balanceDuration;
}

string DurationNode::getRestriction(){
    return this->restriction;
    
}

//SET methods
void DurationNode::setRestriction(string restriction){
    this->restriction=restriction;
}

void DurationNode::setDuration(int duration){
    this->duration=duration;
}

void DurationNode::setId(int id){
    this->id=id;
}


// Add child to list of node's list of children
void DurationNode::setChild(DurationNode child){
    this->children.push_back(child);
    
}

void DurationNode::expand(vector<string>* vectorR, vector<DurationNode>* vectorDN){
    int idChild = this->id + 1;
    //base case #1
    // This case catches the moment that our iterator reaches the end of the vectorDN.
    //checar si tengo que reducir en uno el lado izquierdo de la desigualdad
    if(vectorDN->size() < idChild){
        return;
    }
    else{
        
            //base case #2
        // 
        if(this->balanceDuration < this->duration){
            vectorR->push_back(this->restriction);
            return;
        }
        else{

            for(int i =idChild; i < vectorDN->size(); i++){
            DurationNode info = vectorDN->at(i);
            DurationNode* child = new DurationNode(i, this->duration + info.duration, this->balanceDuration);
            string constraint;
            if(this->restriction.size() == 0)
                constraint = info.getRestriction();
            else
                constraint = this->getRestriction().append(":").append(info.getRestriction());

            child->setRestriction(constraint);

            //sera necesario?
            //this->setChild(child);
            child->expand(vectorR, vectorDN);

            }

        }





    }
    

    
    
    
}

void DurationNode::print(){
    
            
            //cout<< "NODO:" << restriction << endl;
            cout<< "Duration: " << duration << endl;
            cout<< "Id: " << id << endl;
            //cout<< "Balancing Time: " << balanceDuration << endl;
}





/*
 * 
 //I have to use a pointer which locates the node I'm introducing as a parameter, this way I'll add the necessary children.
void DurationNode::setChildren(DurationNode *parent, vector<DurationNode> arr){
    
    for(int i = 1+parent->id; i< arr.size() ;i++){
        parent->setChild(arr.at(i));
            
    }   
    
}
 * 
 * 
 */
