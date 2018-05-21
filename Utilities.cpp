/* 
 * File:   Utilities.cpp
 * Author: juanpablo
 * 
 * Created on June 12, 2017, 11:36 AM
 */

#include "stdafx.h"
#include "Utilities.h"
#include "DurationNode.h"
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

int StrRestr2BinVector(int numStations, string rest, unsigned int * h_vec_p) {

	int control = 0;
	vector<string> vectorsR;
	Tokenize(rest, vectorsR, ",");                           //Separo el VECTOR por los operadores OR
	//cout << "Num Rest: " << vectorsR.size() << endl;

	for (int w = 0; w<vectorsR.size(); w++) {

		vector<string> elementsR;
		Tokenize(vectorsR[w], elementsR, ":");               //Separo vectores de solucion en cada variable binaria
		//cout << "Num elements: " << elementsR.size() << endl;

		for (int k = 0; k<elementsR.size(); k++) {
			int numStaR = 0;
			int numActll= 0;
			int auxx = 0;
			numActll = hashAuxll(actLetter(elementsR[k]));
			numStaR = numStation(elementsR[k]);
			//int vector
			//cout << numStaR + control << endl;
			//cout << "Numero de actividad: " << numActll << endl;
			auxx = *(h_vec_p + numStaR + control);
			*(h_vec_p + numStaR + control) = auxx + numActll;
			//cout << "Valor del arreglo: " << *(h_vec_p + numStaR + control) << endl;
		}

		control = control + numStations;

	}

	return control;

}

void printArrayPointer(unsigned int * p, int size, int numStations) {
	int aux;
	unsigned int aux1;
	for (int i = 0; i < size; i++) {
		if (i%numStations == 0) {
			cout << "SOLUTION VECTOR " << i/numStations << endl;
			cout << "  ABCDEFGHIJKLMNOPQRSTUVWXYZ" << endl;
		}
		//printf("Element %d: %d\n", i, *(p + i));
		//aux = (*(p+i)) + 1073741824;
		//cout << dec2bin(aux) << endl;
		cout << dec2bin(*(p + i)) << endl;
	}
}

//void print_LinMatrix(thrust::device_vector<long long> vec) {
//	for (int i = 0; i < vec.size(); i++) {
//		if (i % 10) {
//			cout << "ANOTHER ONE" << endl;
//		}
//		cout << dec2bin(vec[i]) << endl;
//	}
//
//}


string dec2bin(unsigned int val) {
	//char result[(sizeof(int) * 8) + 1];
	//int index = sizeof(int) * 8;
	//result[index] = '\0';
	//do result[--index] = '0' + (n & 1);
	//while (n >>= 1);
	//return string(result + index);

	char mask[8 * sizeof(unsigned int) + 1] = { 0 };
	for (int j = 0; j < (8 * sizeof(unsigned int) + 1); j++) {
		mask[j] = (val << j) & (1 << (8 * sizeof(unsigned int) - 1)) ? '1' : '0';
	}
	return mask;

}

//string dec2bin(unsigned long long n) {
//	char result[(sizeof(unsigned long long) * 8) + 1];
//	unsigned long long index = sizeof(unsigned long long) * 8;
//	result[index] = '\0';
//	do result[--index] = '0' + (n & 1);
//	while (n >>= 1);
//	return string(result + index);
//}




int round(double a, bool roundUp)
{
	if(roundUp)
	{
		if( ((int) a) == a)
		{
			return (int) a;
		}
		else
		{
			return (int) a + 1;
		}
	}else
	{
		return (int) a;
	}
}


// Devuelve la el numero asociado a una cierta letra de actividad.
int hashAux(string s)
{
    int aux = std::min(1, (int)s.size()-1);
    return 26*(s.at(0)-96) * aux +s.at(s.size()-1)-97;
    
}

int hashAuxll(string s)
{
	int aux = std::min(1, (int)s.size() - 1);
	int aux1 = 26 * (s.at(0) - 96) * aux + s.at(s.size() - 1) - 97;
	return pow(2, 29-aux1);

}

// Devuelve la letra asociada a un numero de actividad
string invHashAux(int num)
{
    string res;
    int p1 =(num%26)+1+96;
    double p2 = floor(num/26)+96;
    
    string s1;
    s1+= p1;
    

    if(p2!=96){
        string s2;
        s2+= p2;
        res.append(s2);
    }
    
    res.append(s1);
    
    return res;
    
  
    
}

// getNegation(Actividad, estacion, tabla, numero de estaciones deseado)
// regresa formato correcto
    
// Formato de entrada para este metodo: Entra una sola actividad y obtenemos la negacion de la misma
// Formato de Salida es las letras de esa actividad que corresponden al complemento del universo de estaciones a las cuales puede ser asiganda ese tr


string getNegation(int activity, int station, int** tableUniverse, int numberStations){
    bool flag= true;
    string str, activityLetter;
    int activityNum = activity;
    activityLetter = invHashAux(activity);
    int i = 0;
    
    while(tableUniverse[i]!= NULL && i < numberStations){
        
        if(i != station && tableUniverse[i][activity] == 1){
            if(!flag){
                str.append(",");
            }
            str.append(activityLetter);
            //???
            str.append(to_string(i)); //como se comvertia un int en su numero para hacer append a un string
            flag=false;
        }
        i++;
    }
    
    return str;
    
}






string constraintNeg(string str, int** tableUniverse, int numberStations){
    
    vector<string> elementsR;
    string temp;
    string aux;
    int numActR, numStaR;
    
    Tokenize(str,elementsR,":");
    
    for(int k=0; k<elementsR.size();k++){
        numActR = hashAux(actLetter(elementsR[k]));
        numStaR = numStation(elementsR[k]);
        aux = getNegation(numActR, numStaR, tableUniverse, numberStations);
        
        if(temp.size() == 0){
            temp.append(aux);
        }
        else{
            temp.append(",");
            temp.append(aux);
        }
    }
    
    return temp;
    
    

    

    
}


vector<string> split(const char *str, char c)
{
    vector<string> result;

    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}


vector<DurationNode> merge(vector<DurationNode> L, vector<DurationNode> R)
{
   vector<DurationNode> arr (L.size()+R.size());
   
    /* Merge the temp arrays back into arr[l..r]*/
    int i = 0; // Initial index of first subarray
    int j = 0; // Initial index of second subarray
    int k = 0; // Initial index of merged subarray
    while (i < L.size() && j < R.size())
    {
        if (L[i].getDuration() >= R[j].getDuration())
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < L.size())
    {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < R.size())
    {
        arr[k] = R[j];
        j++;
        k++;
    }

   return arr;
}

vector<DurationNode> MergeSort(vector<DurationNode> intArr)
{
    vector<DurationNode> recresult;

    // base - if array is of length 1, nothing to do, return it as is
    if (intArr.size() == 1) {
        return intArr;
    } else {
        int mid = intArr.size() / 2;
        // copy left half
        vector<DurationNode> leftArr;
        for(int i=0 ; i<= mid-1; i++){
            leftArr.push_back(intArr[i]);
        }
        // copy right half
        vector<DurationNode> rightArr;
        for(int j = mid; j <= intArr.size()-1; j++){
            rightArr.push_back(intArr[j]);
        }

        leftArr = MergeSort(leftArr);
        rightArr = MergeSort(rightArr);

        recresult = merge(leftArr, rightArr);
    }

    return recresult;
}

//! Tokenize the given string str with given delimiter. If no delimiter is given whitespace is used.
void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ")
{
   tokens.clear();
   // Skip delimiters at beginning.
   std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
   // Find first "non-delimiter".
   std::string::size_type pos = str.find_first_of(delimiters, lastPos);

   while (std::string::npos != pos || std::string::npos != lastPos)
   {
	// Found a token, add it to the vector.
	tokens.push_back(str.substr(lastPos, pos - lastPos));
	// Skip delimiters.  Note the "not_of"
	lastPos = str.find_first_not_of(delimiters, pos);
	// Find next "non-delimiter"
	pos = str.find_first_of(delimiters, lastPos);
   }
}


// no los he agragado al header


string actLetter(string str)
{
    string res;
    int cont = 0;
    int val=str.at(cont);
    string aux;
    
    while( val >= 97){
        aux = str.at(cont);
        res.append(aux);
        cont++;
        val=str.at(cont);
    }
    
    return res;

}

int numStation(string str)
{
    int size = actLetter(str).size();
    int num = stoi(str.substr(size),nullptr,10);
    
    return  num;
}






