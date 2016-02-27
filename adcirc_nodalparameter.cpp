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
#include "adcirc_errors.h"

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for the adcirc_nodalparameter class
 *
 * \author Zach Cobell
 *
 * @param numNodes            [in]            Number of nodes in the mesh so the vectors can be sized
 * @param nodalAttributeName  [in] [optional] Name of the nodal parameter we are creating (default: noName)
 * @param nodalAttributeUnits [in] [optional] Units to denote in the fort.13 file (default: none)
 * @param numValues           [in] [optional] Number of values to assign to a node (default: 1)
 * @param *parent             [in]            Reference to QObject. Enables automatic memory management to avoid memory leaks
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
    this->nNodes = numNodes;
    this->defaultValue.resize(this->nValues);

    //...Set the default values
    for(i=0;i<this->nValues;i++)
        this->defaultValue[i] = 0.0;

    //...Size the vectors to hold the values
    this->values.resize(this->nNodes);
    for(i=0;i<this->nNodes;i++)
        this->values[i].resize(this->nValues);

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to set the default values that will be used in for each nodal parameter
//-----------------------------------------------------------------------------------------//
/** \brief Function to assign default values that will be specified in the fort.13 header
 *
 * \author Zach Cobell
 *
 * @param nodalParameterDefaultValues [in] Vector of doubles containing the default values
 *                                         to be used in each column of the fort.13 file
 *
 * Constructs an adcirc_nodalparameter object. The user should specify some of the vaues found
 * in the fort.13 header here so they are set and retained automatically. Note that this function
 * does not need to be run since the code will automatically assign the most frequently used
 * value to the fort.13 header when it is written. However, if the user wants to specify
 * the values to be used in the header manually, they can do so here.
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_nodalparameter::setDefaultValues(QVector<double> nodalParameterDefaultValues)
{
    if(nodalParameterDefaultValues.length()!=this->nValues)
        return ERROR_NODALPARAM_DEFAULTVALUESIZE;

    for(int i = 0;i<this->nValues;i++)
        this->defaultValue[i] = nodalParameterDefaultValues[i];

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to read the body from the fort.13 file into this nodal attribute
//-----------------------------------------------------------------------------------------//
/** \brief Function to read and assign the body of the fort.13 file to the vectors in this class
 *
 * \author Zach Cobell
 *
 * @param fileData [in] Body of the fort.13 file for this nodal attribute
 *
 * This function reads the portion of the body of a fort.13 that contains the non-default
 * values to assign nodes in the ADCIRC mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_nodalparameter::read(QStringList fileData)
{
    QString tempString;
    QStringList tempList;
    int i,j,index;
    double tempDouble;
    bool err;

    for(i=0;i<fileData.length();i++)
    {
        tempString = fileData[i];
        tempList   = tempString.simplified().split(" ");
        if(tempList.length()-1!=this->nValues)
            return ERROR_NODALPARAM_NOTENOUGHVALUES;

        tempString = tempList.value(0);
        index      = tempString.toInt(&err);
        if(!err)
            return ERROR_NODALPARAM_READERROR;

        if(index<=0 || index>this->nNodes)
            return ERROR_NODALPARAM_OUTOFRANGE;

        for(j=0;j<this->nValues;j++)
        {
            tempString             = tempList.value(j+1);
            tempDouble             = tempString.toDouble(&err);
            if(!err)
                return ERROR_NODALPARAM_READERROR;
            this->values[index-1][j] = tempDouble;
        }
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
