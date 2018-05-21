/* 
 * File:   Mixer.cpp
 * Author: normalazo
 * 
 * Created on September 3, 2017, 6:21 PM
 */

#include "Mixer.h"

Mixer::Mixer()
{
    
}

string Mixer::mix(string Temp, string R)
{
    //es mejor declarar las variables aqui?
    int numActR=-1;
    int numStaR=-1;
    int numAct=-1;
    int numSta=-1;
    string constr;
    string toString;
    bool flag = true;
    bool found = false;
    string res = "";
    
    
    

        // R
        
        vector<string> vectorsR;
        Tokenize(R,vectorsR,",");
        for(int w=0; w<vectorsR.size();w++){ 
            vector<int> auxR (701,-1);
            vector<string> elementsR;
            Tokenize(vectorsR[w],elementsR, ":");
            for(int k=0; k<elementsR.size();k++){
                numActR = hashAux(actLetter(elementsR[k]));
                numStaR = numStation(elementsR[k]);
                auxR[numActR]=numStaR;
            }
            
            vector<string> vectors;
            Tokenize(Temp, vectors, ",");
            for(int i=0;i<vectors.size();i++){
                vector<string> elements;
                Tokenize(vectors[i],elements,":");
                vector<int> auxTemp = auxR;
                int j=0;
                while(j<elements.size() && flag){
                    numAct = hashAux(actLetter(elements[j]));
                    numSta = numStation(elements[j]);

                    if(auxTemp[numAct] != numSta){ //diferente estacion
                        if(auxTemp[numAct] >= 0){ //no es nulo
                            flag=false;
                        }
                        else{
                            auxTemp[numAct] = numSta;
                        }
                    }
                    j++;
                }//end while para el vector i

                if(flag){
                    constr = "";
                    for(int k=0; k<auxTemp.size();k++){
                        if(auxTemp[k] >= 0){
                            if(constr.size() == 0){
                                toString = invHashAux(k);
                                constr.append(toString);
                                toString = to_string(auxTemp[k]);
                                constr.append(toString);
                            }
                            else{
                                constr.append(":");
                                toString = invHashAux(k);
                                constr.append(toString);
                                toString = to_string(auxTemp[k]);
                                constr.append(toString);

                            }
                        }

                    } //fin de for para crear string de constraint.
                    

                    if(res.size() == 0){
                        res.append(constr);

                    }
                    else{
                        //aqui puede estar el pedo
                        
                        
                        
                        string auxFind;
                        int pos;
                        
                        auxFind.append("");
                        auxFind.append(constr);
                        auxFind.append("");
                        
                        pos = res.find(auxFind);
                        found = (pos!=std::string::npos);
                        if(!found){
                            res.append(",");
                            res.append(constr);
                        }
                        else
                        {

                            found = false;
                            auxFind = "";
                            // checar entre comas
                            auxFind.append(",");
                            auxFind.append(constr);
                            auxFind.append(",");
                            found = (res.find(auxFind)!=std::string::npos);
                            int constr_size = constr.size();
                            string auxConstr;
                            auxFind = "";
                            // checar entre comas
                            auxFind.append(",");
                            auxFind.append(constr);
                            
                                              
                            if(!found && pos!=0){
                                string lastConstr = res.substr(res.size()-constr_size-1, res.size()-1);
                                if(auxFind.compare(lastConstr)!=0){
                                        res.append(",");
                                        res.append(constr);
                                    
                                }

                            }
                            
                        }
                    }

                } //termina if que uso para decidir si la restriccion va a entrar a mi solucion temporal.
                flag = true;

            }//end for de recorrer vectores de temp
         
            
            
            
        }
        
    return res;
    
}

//despues de integrar una letra aparentmente la tiene que tener ahuevo 
        



