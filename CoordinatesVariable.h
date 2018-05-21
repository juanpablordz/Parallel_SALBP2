/* 
 * File:   CoordinatesVariable.h
 * Author: juanpablo
 *
 * Created on June 13, 2017, 10:07 AM
 */

#ifndef COORDINATESVARIABLE_H
#define COORDINATESVARIABLE_H

class CoordinatesVariable
{
public:
	CoordinatesVariable();
	void updateVarFamIndex(int varFamIndex);
	void updateVarIndex(int varIndex);
	int getVarFamIndex();
	int getVarIndex();

private:
	int varFamIndex;
	int varIndex;
};

#endif