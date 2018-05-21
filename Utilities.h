/* 
 * File:   Utilities.h
 * Author: juanpablo
 *
 * Created on June 12, 2017, 11:36 AM
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include "DurationNode.h"
#include <thrust/host_vector.h> 
#include <thrust/device_vector.h> 
using namespace std;


//Transformacion de restricciones a matrices linealizadas en vectores 
int StrRestr2BinVector(int numStations, string rest, unsigned int * h_vec_p);
void printArrayPointer(unsigned int * p, int size, int numStations);

string dec2bin(unsigned int val);
int round(double a, bool roundUp);
int hashAux(string s);
int hashAuxll(string s);
string invHashAux(int num);
string getNegation(int activity, int station, int** tableUniverse, int numberSations);
vector<string> split(const char *str, char c);
vector<DurationNode> MergeSort(vector<DurationNode> intArr);
vector<DurationNode> Merge(vector<DurationNode> L, vector<DurationNode> R);
void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);

string constraintNeg(string str, int** tableUniverse, int numberStations);
string actLetter(string str);
int numStation(string str);





#endif