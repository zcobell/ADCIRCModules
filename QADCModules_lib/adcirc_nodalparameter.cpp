/*-------------------------------GPL-------------------------------------//
//
// QADCModules - A library for working with ADCIRC models
// Copyright (C) 2016  Zach Cobell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------*/

#include "adcirc_nodalparameter.h"
#include "adcirc_nodalattribute.h"
#include "QADCModules_errors.h"
#include <QDebug>

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for the adcirc_nodalparameter class
 *
 * @param[in] numNodes                        Number of nodes in the mesh so the vectors can be sized
 * @param[in] nodalAttributeName   [optional] Name of the nodal parameter we are creating (default: noName)
 * @param[in] nodalAttributeUnits  [optional] Units to denote in the fort.13 file (default: none)
 * @param[in] numValues            [optional] Number of values to assign to a node (default: 1)
 * @param[in] *parent                         Reference to QObject. Enables automatic memory management to avoid memory leaks
 *
 * Constructs an adcirc_nodalparameter object. The user should specify some of the vaues found
 * in the fort.13 header here so they are set and retained automatically. This function also
 * sets the default value for a nodal parameter specified in the header to zero.
 *
 **/
//-----------------------------------------------------------------------------------------//
adcirc_nodalparameter::adcirc_nodalparameter(int numNodes, QString nodalAttributeName,
                                             QString nodalAttributeUnits, int numValues,
                                             QObject *parent) : QObject(parent)
{
    int i;

    //...Set the values inside the class
    this->name = nodalAttributeName;
    this->units = nodalAttributeUnits;
    this->nValues = numValues;
    this->defaultValue.resize(this->nValues);

    //...Set the default values
    for(i=0;i<this->nValues;i++)
        this->defaultValue[i] = 0.0;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to set the default values that will be used in for each nodal parameter
//-----------------------------------------------------------------------------------------//
/** \brief Function to assign default values that will be specified in the fort.13 header
 *
 * @param[in] nodalParameterDefaultValues  Vector of doubles containing the default values
 *                                         to be used in each column of the fort.13 file
 *
 * Function to assign default values that will be specified in the fort.13 header
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_nodalparameter::setDefaultValues(QVector<qreal> nodalParameterDefaultValues)
{
    if(nodalParameterDefaultValues.size()!=this->nValues)
        return ERROR_NODALPARAM_DEFAULTVALUESIZE;

    for(int i = 0;i<this->nValues;i++)
        this->defaultValue[i] = nodalParameterDefaultValues[i];

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to determine the optimum value for the fort.13 header for this parameter
//-----------------------------------------------------------------------------------------//
/** \brief Function to determine the optimum value for the fort.13 header for this parameter
 *
 * @param[in] nodeData vector of adcirc_nodalattribute pointers to find the default value for
 *
 * Function to determine the optimum value for the fort.13 header for this parameter
 *
 **/
//-----------------------------------------------------------------------------------------//
qreal adcirc_nodalparameter::getDefaultValue(QVector<adcirc_nodalattribute*> nodeData)
{
    int i,index,maxNum;
    qreal optimumValue;
    QMap<qreal,int> valueMap;
    QList<qreal>    valueList;
    QVector<int>     count;

    index = 0;
    maxNum = 0;

    //...Build a list of unique values
    for(i=0;i<nodeData.length();i++)
    {
        if(!valueList.contains(nodeData[i]->values[0]))
        {
            valueList.append(nodeData[i]->values[0]);
            valueMap[nodeData[i]->values[0]] = index;
            index = index + 1;
        }
    }

    //...Generate a vector to count the values
    count.resize(valueList.length());

    //...Total the hits on each value
    for(i=0;i<nodeData.length();i++)
        count[valueMap[nodeData[i]->values[0]]] = count[valueMap[nodeData[i]->values[0]]] + 1;

    //...Find the maximum number of hits
    for(i=0;i<count.size();i++)
    {
        if(count[i]>maxNum)
        {
            maxNum = count[i];
            index  = i;
        }
    }

    //...Get the determined optimium value
    optimumValue = valueList.at(index);

    return optimumValue;
}
//-----------------------------------------------------------------------------------------//
