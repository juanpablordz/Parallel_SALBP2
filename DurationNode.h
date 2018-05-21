/* 
 * File:   DurationNode.h
 * Author: normalazo
 *
 * Created on August 10, 2017, 10:59 PM
 */

#ifndef DURATIONNODE_H
#define	DURATIONNODE_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;


// This node class will be the the fundamental element in the directed graph that will
// help us determine the boolean restriction that exist in certain station.

class DurationNode {
public:
    
    
    int duration;
    int id;
    int activity;
    int balanceDuration;
    
    // This attribute corresponds to the concatenation of the nodes trajectory building the possible 
    // restriction in the correct format.
    string restriction;

    //List of DurationNodes that are part of the directed graph implementation
    vector<DurationNode> children;

    
    DurationNode(int id, int duration);
    DurationNode(int id, int duration, int balanceDuration);
    DurationNode(int duration, string restriction, int balanceDuration);
    DurationNode();
    
    //accumulated duration

    //GETTERS
    DurationNode getChild(int i);
    int getId();
    int getDuration();
    string getRestriction();
    int getBalanceDuration();
    
    //SETTERS
    void setId(int id);
    void setChild(DurationNode child);
    void setChildren(DurationNode* parent, vector<DurationNode> arr);
    void setRestriction(string restriction);
    void setDuration(int duration);
    
    //Recursive function
    void expand(vector<string>* vectorR, vector<DurationNode>* vectorDN);
    void print();
    
    
    
private:



};

#endif	/* DURATIONNODE_H */

