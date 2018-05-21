/* 
 * File:   Variable.cpp
 * Author: juanpablo
 * 
 * Created on June 13, 2017, 10:06 AM
 */

#include <iostream>
#include "stdafx.h"
#include "Variable.h"
#include "Utilities.h"

Variable::Variable(Activity * assocActivity, Station * assocStation)
{
	Variable::assocActivity=assocActivity;
	Variable::assocStation=assocStation;
	flag=0;
}

Variable::Variable()
{

}


void Variable::setFlag(bool value)
{
	flag= value?1:0;
}

bool Variable::isOn()
{
	return flag==1;
}

double Variable::getValue()
{
	return flag*assocActivity->duration;
}

double Variable::getDuration()
{
	return assocActivity->duration;
}

int Variable::getActivityNumber()
{
	return assocActivity->number;
}

int Variable::getStationNumber()
{
	return assocStation->number;
}

void Variable::print()

/*
 * En esta seccion lo que queremos es que parezca primero la letra asociada con la Actividad
 * que esta en assocActivity->number. Despues la estacion asociada en numero que se 
 * encuentra en assocStation->number.
 */

{
    string str;
    
    str = invHashAux(assocActivity->number);
    
    cout << str << assocStation->number;
}

string Variable::getActSta()
{
    string str;
    str = invHashAux(assocActivity->number);
    str.append(to_string(assocStation->number));
    
    return str;
}