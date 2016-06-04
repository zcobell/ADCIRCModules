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
#include <QFile>

#include "adcirc_mesh.h"
#include "adcirc_node.h"
#include "adcirc_fort13.h"
#include "adcirc_nodalparameter.h"
#include "adcirc_nodalattribute.h"
#include "QADCModules_errors.h"

//-----------------------------------------------------------------------------------------//
// Constructor
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_fort13(QObject *parent)
 * \brief Constructs an adcirc_fort13 object
 *
 * @param[in] *mesh    Reference to an adcirc_mesh object
 * @param[in] *parent  Reference to QObject. Enables automatic memory management to avoid memory leaks
 *
 * Constructs an adcirc_fort13 object
 *
 **/
//-----------------------------------------------------------------------------------------//
adcirc_fort13::adcirc_fort13(adcirc_mesh *mesh, QObject *parent) : QObject(parent)
{
    this->error = new QADCModules_errors(this);
    this->mesh  = mesh;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//
//
//   P U B L I C
//             F U N C T I O N S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Publicly available read method
//-----------------------------------------------------------------------------------------//
/** \brief Publicly available method to read an ADCIRC fort.13 file
 *
 * @param[in] filename Filename of the fort.13 file to be read
 *
 * Publicly available method to read an ADCIRC fort.13 file
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_fort13::read(QString inputFilename)
{
    int ierr;

    this->filename = inputFilename;

    if(this->filename==QString())
    {
        this->error->setError(ERROR_NULLFILENAM);
        return this->error->getError();
    }

    QFile nodalAttributesFile(this->filename);

    if(!nodalAttributesFile.exists())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
    }

    ierr = this->readNodalAttributesFile();

    this->error->setError(ierr);

    return ierr;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Publicly available write method
//-----------------------------------------------------------------------------------------//
/** \brief Publicly available method to write an ADCIRC fort.13 file
 *
 * @param[in] outputFile                 Filename of the fort.13 file to be written
 * @param[in] userSpecifiedDefaultValues If true, the optimization of the fort.13 default value
 *                                       is not performed
 *
 * Publicly available method to write an ADCIRC fort.13 file
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_fort13::write(QString outputFilename, bool userSpecifiedDefaultValues)
{
    if(outputFilename==QString())
    {
        this->error->setError(ERROR_NULLFILENAM);
        return this->error->getError();
    }

    int ierr = this->writeNodalAttributesFile(outputFilename,userSpecifiedDefaultValues);

    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//




//-----------------------------------------------------------------------------------------//
//
//
//   P R O T E C T E D
//             F U N C T I O N S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// method to do the heavy lifting to read an ADCIRC fort.13 file
//-----------------------------------------------------------------------------------------//
/** \brief method used internally to read an ADCIRC nodal attributes file
 *
 * method used internally to read an ADCIRC nodal attributes file
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_fort13::readNodalAttributesFile()
{
    int ierr,i,j,index,position,tempInt;
    int tempNumVals,numNonDefault;
    QVector<qreal> tempDefaultValue;
    bool err;
    QByteArray fileByteData;
    QString fileStringData,tempString,tempUnits,tempName;
    QStringList fileStringList,tempStringList;
    QFile nodalAttributesFile(this->filename);

    //...Open the file
    if(!nodalAttributesFile.open(QIODevice::ReadOnly))
        return ERROR_FILEOPENERR;

    //...Read the data
    fileByteData = nodalAttributesFile.readAll();
    fileStringData = QString(fileByteData);
    fileStringList = fileStringData.split("\n");
    fileByteData = QByteArray();
    fileStringData = QString();

    //...Read the header
    position = 0;
    this->title = fileStringList.value(0);

    tempString = fileStringList.value(1);
    tempInt = tempString.toInt(&err);
    if(!err)
        return ERROR_NODALATT_HEADERR;
    this->numNodes = tempInt;

    tempString = fileStringList.value(2);
    tempInt = tempString.toInt(&err);
    if(!err)
        return ERROR_NODALATT_HEADERR;
    this->numParameters = tempInt;

    this->nodalParameters.resize(this->numParameters);
    this->nodalData.resize(this->numParameters);

    //...Read the nodal attributes and their initialization data
    position = 3;
    if(position>fileStringList.length()-1)
    {
        this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
        return this->error->getError();
    }
    for(i=0;i<this->numParameters;i++)
    {
        tempString  = fileStringList.value(position);
        tempName    = tempString.simplified();

        position    = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
            return this->error->getError();
        }
        tempString  = fileStringList.value(position);
        tempUnits   = tempString.simplified();

        position    = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
            return this->error->getError();
        }
        tempString  = fileStringList.value(position);
        tempNumVals = tempString.toInt(&err);
        if(!err)
        {
            this->error->setError(ERROR_NODALATT_HEADERR);
            return this->error->getError();
        }

        position         = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
            return this->error->getError();
        }
        tempString       = fileStringList.value(position);
        tempStringList   = tempString.simplified().split(" ");

        if(tempNumVals!=tempStringList.length())
        {
            this->error->setError(ERROR_NODALATT_HEADERR);
            return this->error->getError();
        }

        tempDefaultValue.resize(tempNumVals);

        for(j=0;j<tempNumVals;j++)
        {
            tempString          = tempStringList.value(j);
            tempDefaultValue[j] = tempString.toDouble(&err);

            if(!err)
            {
                this->error->setError(ERROR_NODALATT_HEADERR);
                return this->error->getError();
            }
        }

        this->nodalParameters[i] = new adcirc_nodalparameter(this->numNodes,tempName,tempUnits,tempNumVals,this);
        this->nodalParameters[i]->setDefaultValues(tempDefaultValue);
        this->attributeLocations[tempName] = i;

        position = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
            return this->error->getError();
        }

    }


    //...Read the body of the nodal attributes file
    for(i=0;i<this->numParameters;i++)
    {
        tempString    = fileStringList.value(position);
        tempName      = tempString.simplified();
        index         = this->attributeLocations[tempName];
        position      = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
            return this->error->getError();
        }
        tempString    = fileStringList.value(position);
        numNonDefault = tempString.toDouble(&err);
        if(!err)
        {
            this->error->setError(ERROR_NODALATT_BODYERR);
            return this->error->getError();
        }

        tempStringList = QStringList();
        position       = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
            return this->error->getError();
        }

        for(j=0;j<numNonDefault;j++)
        {
            tempStringList.append(fileStringList.value(position));
            position = position + 1;
            if(position>fileStringList.length()-1)
            {
                this->error->setError(ERROR_NODALATT_UNEXPECTEDEND);
                return this->error->getError();
            }
        }

        this->nodalData[i].resize(this->numNodes);
        for(j=0;j<this->numNodes;j++)
            this->nodalData[i][j] = new adcirc_nodalattribute(this->nodalParameters[index],this);

        ierr = this->readNodalData(i,tempStringList);
        if(ierr!=ERROR_NOERROR)
        {
            this->error->setError(ierr);
            return this->error->getError();
        }
    }

    if(this->mesh!=NULL)
        this->mapNodalAttributesToMesh();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// method to do the heavy lifting to write an ADCIRC fort.13 file
//-----------------------------------------------------------------------------------------//
/** \brief method used internally to write an ADCIRC nodal attributes file
 *
 * method used internally to write an ADCIRC nodal attributes file
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_fort13::writeNodalAttributesFile(QString outputFilename, bool userSpecifiedDefaultValues)
{
    int i,j;
    QString tempString,tempString2;
    QStringList tempStringList;

    QFile outputFile(outputFilename);
    if(!outputFile.open(QIODevice::WriteOnly))
        return ERROR_FILEOPENERR;

    QTextStream out(&outputFile);

    //...If the user hasn't requested so, generate a list of default values
    //   Note: This only works for one value, not for nodal parameters with more
    //   than one value per node
    if(!userSpecifiedDefaultValues)
        for(i=0;i<this->numParameters;i++)
            if(this->nodalParameters[i]->nValues==1)
                this->nodalParameters[i]->defaultValue[0] = this->nodalParameters[i]->getDefaultValue(this->nodalData[i]);

    //...Write the header
    out << this->title << "\n";

    tempString.sprintf("%11i",this->numNodes);
    out << tempString << "\n";

    tempString.sprintf("%11i",this->numParameters);
    out << tempString << "\n";

    //...Write the default values for each nodal parameters
    for(i=0;i<this->numParameters;i++)
    {
        out << this->nodalParameters[i]->name << "\n";
        out << this->nodalParameters[i]->units << "\n";

        tempString = QString();
        tempString.sprintf("%11i",this->nodalParameters[i]->nValues);
        out << tempString << "\n";

        tempString = QString();
        for(j=0;j<this->nodalParameters[i]->nValues;j++)
        {
            tempString2 = QString();
            tempString2.sprintf("%12.6f",this->nodalParameters[i]->defaultValue[j]);
            tempString = tempString + "  " + tempString2;
        }
        out << tempString << "\n";
    }

    //...Write the body section of the fort.13
    for(i=0;i<this->numParameters;i++)
    {
        out << this->nodalParameters[i]->name << "\n";

        tempString.sprintf("%11i",this->getNumNonDefault(this->nodalParameters[i],this->nodalData[i]));
        out << tempString << "\n";

        tempStringList = QStringList();
        tempStringList = this->writeNodalParameter(i);
        for(j=0;j<tempStringList.length();j++)
            out << tempStringList.value(j) << "\n";
    }

    outputFile.close();

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
// method to read the body from the fort.13 file into this nodal attribute
//-----------------------------------------------------------------------------------------//
/** \brief method to read and assign the body of the fort.13 file to the vectors in this class
 *
 * @param[in] data Body of the fort.13 file for this nodal attribute
 *
 * This method reads the portion of the body of a fort.13 that contains the non-default
 * values to assign nodes in the ADCIRC mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_fort13::readNodalData(int nodalAttributeIndex, QStringList &data)
{
    QString tempString;
    QStringList tempList;
    int i,j,index;
    qreal tempDouble;
    bool err;

    for(i=0;i<this->numNodes;i++)
        for(j=0;j<this->nodalParameters[nodalAttributeIndex]->nValues;j++)
            this->nodalData[nodalAttributeIndex][i]->values.fill(this->nodalParameters[nodalAttributeIndex]->defaultValue[j]);

    for(i=0;i<data.length();i++)
    {
        tempString = data[i];
        tempList   = tempString.simplified().split(" ");
        if(tempList.length()-1!=this->nodalParameters[nodalAttributeIndex]->nValues)
            return ERROR_NODALPARAM_NOTENOUGHVALUES;

        tempString = tempList.value(0);
        index      = tempString.toInt(&err);
        if(!err)
            return ERROR_NODALPARAM_READERROR;

        if(index<=0 || index>this->numNodes)
            return ERROR_NODALPARAM_OUTOFRANGE;

        for(j=0;j<nodalParameters[nodalAttributeIndex]->nValues;j++)
        {
            tempString             = tempList.value(j+1);
            tempDouble             = tempString.toDouble(&err);
            if(!err)
                return ERROR_NODALPARAM_READERROR;
            this->nodalData[nodalAttributeIndex][index-1]->values[j] = tempDouble;
            this->nodalData[nodalAttributeIndex][index-1]->metadata  = this->nodalParameters[nodalAttributeIndex];
        }

    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Method to add the nodal attributes pointers to the adcirc_mesh object
//-----------------------------------------------------------------------------------------//
/** \brief Method to add the adcirc_nodalattribute pointers to the adcirc_mesh object
 *
 * Method to add the adcirc_nodalattribute pointers to the adcirc_mesh object
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_fort13::mapNodalAttributesToMesh()
{
    for(int i=0;i<this->mesh->numNodes;i++)
    {
        this->mesh->nodes[i]->nodalData.resize(this->numParameters);
        for(int j=0;j<this->numParameters;j++)
            this->mesh->nodes[i]->nodalData[j] = this->nodalData[j][i];
    }
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to create the text to be written to the nodal attributes file for this parameter
//-----------------------------------------------------------------------------------------//
/** \brief Function to create the text to be written to the nodal attributes file for this parameter
 *
 * @param[in] index
 *
 * Function to create the text to be written to the nodal attributes file for this parameter
 *
 **/
//-----------------------------------------------------------------------------------------//
#include <QDebug>
QStringList adcirc_fort13::writeNodalParameter(int index)
{
    int i,j;
    QString tempLine,tempLine2;
    QStringList outputData;
    bool isNonDefault;

    //...Loop over nodes to generate the fort.13 body
    //   for this nodal attribute
    for(i=0;i<this->numNodes;i++)
    {
        tempLine = QString();
        if(this->nodalParameters[index]->nValues==1)
        {
            if(this->nodalData[index][i]->values[0]!=this->nodalParameters[index]->defaultValue[0])
            {
                tempLine.sprintf("%11i  %12.6f",i+1,this->nodalData[index][i]->values[0]);
                outputData.append(tempLine);
            }
        }
        else
        {
            isNonDefault = false;
            tempLine.sprintf("%11i",i+1);
            for(j=0;j<this->nodalParameters[index]->nValues;j++)
            {
                tempLine2 = QString();
                tempLine2.sprintf("%12.6f",this->nodalData[index][i]->values[j]);
                tempLine = tempLine + "  " + tempLine2;

                if(this->nodalData[index][i]->values[j]==this->nodalParameters[index]->defaultValue[j])
                    isNonDefault = true;
            }

            if(isNonDefault)
                outputData.append(tempLine);
        }
    }

    return outputData;

}
//-----------------------------------------------------------------------------------------//





//-----------------------------------------------------------------------------------------//
// Function to determine the number of non-default nodes
//-----------------------------------------------------------------------------------------//
/** \brief Function to determine the number of non-default nodes
 *
 * @param[in] nodalParam adcirc_nodalparameter object that has the default value we are checking
 * @param[in] nodalAtt   adcirc_nodalattribute object that we will count default values for
 *
 * Function to determine the number of non-default nodes
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_fort13::getNumNonDefault(adcirc_nodalparameter *nodalParam, QVector<adcirc_nodalattribute*> nodalAtt)
{
    int i,j,numNonDefault;
    bool isNonDefault;

    numNonDefault = 0;

    for(i=0;i<this->numNodes;i++)
    {
        isNonDefault = false;
        for(j=0;j<nodalParam->nValues;j++)
        {
            if(nodalAtt[i]->values[j]!=nodalParam->defaultValue[j])
            {
                isNonDefault = true;
                break;
            }
        }
        if(isNonDefault)
            numNonDefault = numNonDefault + 1;
    }

    return numNonDefault;
}
//-----------------------------------------------------------------------------------------//
