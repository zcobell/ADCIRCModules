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

#include "adcirc_nodalattributes.h"
#include "adcirc_errors.h"

//-----------------------------------------------------------------------------------------//
// Constructor
//-----------------------------------------------------------------------------------------//
/** \brief Constructs an adcirc_nodalattributes object
 *
 * \author Zach Cobell
 *
 * @param *parent  [in] Reference to QObject. Enables automatic memory management to avoid memory leaks
 *
 * Constructs an adcirc_nodalattributes object
 *
 **/
//-----------------------------------------------------------------------------------------//
adcirc_nodalattributes::adcirc_nodalattributes(QObject *parent) : QObject(parent)
{
    this->error = new adcirc_errors(this);
}




//-----------------------------------------------------------------------------------------//
//
//
//   P U B L I C
//             F U N C T I O N S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Publicly available read function
//-----------------------------------------------------------------------------------------//
/** \brief Publicly available function to read an ADCIRC fort.13 file
 *
 * \author Zach Cobell
 *
 * @param filename [in] filename of the fort.13 file to be read
 *
 * Constructs an adcirc_nodalattributes object
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_nodalattributes::read(QString inputFilename)
{
    int ierr;

    this->filename = inputFilename;

    if(this->filename==QString())
        return ERROR_NULLFILENAM;

    QFile nodalAttributesFile(this->filename);

    if(!nodalAttributesFile.exists())
        return ERROR_FILENOEXIST;

    ierr = this->readNodalAttributesFile();

    this->error->errorCode = ierr;

    return ierr;

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
// Function to do the heavy lifting to read an ADCIRC fort.13 file
//-----------------------------------------------------------------------------------------//
/** \brief Function used internally to read an ADCIRC nodal attributes file
 *
 * \author Zach Cobell
 *
 * Function used internally to read an ADCIRC nodal attributes file
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_nodalattributes::readNodalAttributesFile()
{
    int ierr,i,j,index,position,tempInt;
    int tempNumVals,numNonDefault;
    QVector<double> tempDefaultValue;
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

    //...Read the nodal attributes and their initialization data
    position = 3;
    if(position>fileStringList.length()-1)
    {
        this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
        return this->error->errorCode;
    }
    for(i=0;i<this->numParameters;i++)
    {
        tempString  = fileStringList.value(position);
        tempName    = tempString.simplified();

        position    = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
            return this->error->errorCode;
        }
        tempString  = fileStringList.value(position);
        tempUnits   = tempString.simplified();

        position    = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
            return this->error->errorCode;
        }
        tempString  = fileStringList.value(position);
        tempNumVals = tempString.toInt(&err);
        if(!err)
        {
            this->error->errorCode = ERROR_NODALATT_HEADERR;
            return this->error->errorCode;
        }

        position         = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
            return this->error->errorCode;
        }
        tempString       = fileStringList.value(position);
        tempStringList   = tempString.simplified().split(" ");

        if(tempNumVals!=tempStringList.length())
        {
            this->error->errorCode = ERROR_NODALATT_HEADERR;
            return this->error->errorCode;
        }

        tempDefaultValue.resize(tempNumVals);

        for(j=0;j<tempNumVals;j++)
        {
            tempString          = tempStringList.value(j);
            tempDefaultValue[j] = tempString.toDouble(&err);

            if(!err)
            {
                this->error->errorCode = ERROR_NODALATT_HEADERR;
                return this->error->errorCode;
            }
        }

        this->nodalParameters[i] = new adcirc_nodalparameter(this->numNodes,tempName,tempUnits,tempNumVals,this);
        this->attributeLocations[tempName] = i;

        position = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
            return this->error->errorCode;
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
            this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
            return this->error->errorCode;
        }
        tempString    = fileStringList.value(position);
        numNonDefault = tempString.toDouble(&err);
        if(!err)
        {
            this->error->errorCode = ERROR_NODALATT_BODYERR;
            return this->error->errorCode;
        }

        tempStringList = QStringList();
        position       = position + 1;
        if(position>fileStringList.length()-1)
        {
            this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
            return this->error->errorCode;
        }

        for(j=0;j<numNonDefault;j++)
        {
            tempStringList.append(fileStringList.value(position));
            position = position + 1;
            if(position>fileStringList.length()-1)
            {
                this->error->errorCode = ERROR_NODALATT_UNEXPECTEDEND;
                return this->error->errorCode;
            }
        }

        ierr = this->nodalParameters[index]->read(tempStringList);
        if(ierr!=ERROR_NOERROR)
        {
            this->error->errorCode = ierr;
            return this->error->errorCode;
        }
    }

    return ERROR_NOERROR;
}
