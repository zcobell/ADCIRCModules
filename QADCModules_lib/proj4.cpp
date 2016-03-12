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
#include "proj4.h"
#include "proj_api.h"

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for the proj4 wrapper class
 *
 * \author Zach Cobell
 *
 * @param[in] *parent reference to QObject. Enables automatic memory management to avoid memory leaks
 *
 * Constructs an object used to convert coordinates. The initialization function is run at
 * startup which parses the included EPSG file (:/rsc/epsg) to determine the parameters to pass
 * to the Proj4 API
 *
 **/
//-----------------------------------------------------------------------------------------//
proj4::proj4(QObject *parent) : QObject(parent)
{
    this->readEPSGInitializations();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to initialize the EPSG codes
//-----------------------------------------------------------------------------------------//
/** \brief Function to initlize the EPSG codes
 *
 * \author Zach Cobell
 *
 * Initizalizes the EPSG table for use with the Proj4 API
 *
 **/
//-----------------------------------------------------------------------------------------//
int proj4::readEPSGInitializations()
{
    int i;
    QFile epsgFile(":/rsc/epsg");
    QByteArray epsgByteArray;
    QStringList epsgList,tempStringList;
    QString epsgString,tempString;
    QVector<int> epsgIDs;
    QVector<QString> epsgInitStrings;
    QChar hash(35);

    //...Attempt to open the file
    if(!epsgFile.open(QIODevice::ReadOnly))
        return ERROR_PROJ4_CANNOTREADEPSGFILE;

    //...Read the contents of the EPSG file
    epsgByteArray = epsgFile.readAll();
    epsgFile.close();

    //...Convert to string
    epsgString = QString(epsgByteArray);

    //...Split the EPSG data into lines
    epsgList = epsgString.split("\n");

    //...Clean Up
    epsgString = QString();
    epsgByteArray = QByteArray();

    //...Loop over the file
    for(i=0;i<epsgList.length();i++)
    {
        epsgString = epsgList.value(i);
        epsgString = epsgString.simplified();

        //...If there is a valid line (not comment line)
        //   build out the array
        if(epsgString!=QString())
        {
            if(epsgString.at(0)==hash)
                continue;
            else
            {
                tempStringList = epsgString.split(">");
                tempString     = tempStringList.value(0);
                tempString     = tempString.mid(1);

                epsgIDs.resize(epsgIDs.size()+1);
                epsgInitStrings.resize(epsgInitStrings.size()+1);
                epsgIDs[epsgIDs.size()-1] = tempString.toInt();

                tempString     = tempStringList.value(1);
                tempString     = tempString.mid(0,tempString.length()-2);
                tempString     = tempString.simplified();

                epsgInitStrings[epsgInitStrings.size()-1] = tempString;

            }
        }

    }

    //...Build the map between EPSGs and their parameters
    for(i=0;i<epsgIDs.size();i++)
        this->epsgMapping[epsgIDs[i]] = epsgInitStrings[i];

    return ERROR_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to execute a coordinate system transformation using Proj4
//-----------------------------------------------------------------------------------------//
/** \brief Function to execute a coordinate system transformation using Proj4
 *
 * \author Zach Cobell
 *
 * @param[in]  inputEPSG  EPSG that coordinates are currently in
 * @param[in]  outputEPSG EPSG that the coordinates will be converted to
 * @param[in]  input      QPointF object containing the location to be converted
 * @param[out] output     QPointF object containing the converted coordiantes
 * @param[out] isLatLon   Bool that determine if the coordinates are lat/lon or otherwise
 *
 * Function to execute a coordinate system transformation using Proj4
 *
 **/
//-----------------------------------------------------------------------------------------//
int proj4::transform(int inputEPSG, int outputEPSG, QPointF &input, QPointF &output, bool &isLatLon)
{
    projPJ inputPJ,outputPJ;
    double x,y,z;
    int ierr;

    ierr = 0;
    z = 0.0;

    if(!this->epsgMapping.contains(inputEPSG))
        return ERROR_PROJ4_NOSUCHPROJECTION;

    if(!this->epsgMapping.contains(outputEPSG))
        return ERROR_PROJ4_NOSUCHPROJECTION;

    QString currentInitialization = this->epsgMapping[inputEPSG];
    QString outputInitialization  = this->epsgMapping[outputEPSG];

    if(!(inputPJ = pj_init_plus(currentInitialization.toLatin1().data())))
        return ERROR_PROJ4_INTERNAL;

    if(!(outputPJ = pj_init_plus(outputInitialization.toLatin1().data())))
        return ERROR_PROJ4_INTERNAL;

    if(pj_is_latlong(inputPJ))
    {
        x = input.x()*DEG_TO_RAD;
        y = input.y()*DEG_TO_RAD;
    }
    else
    {
        x = input.x();
        y = input.y();
    }

    ierr = pj_transform(inputPJ,outputPJ,1,1,&x,&y,&z);

    if(ierr!=0)
        return ERROR_PROJ4_INTERNAL;

    if(pj_is_latlong(outputPJ))
    {
        output.setX(x*RAD_TO_DEG);
        output.setY(y*RAD_TO_DEG);
        isLatLon = true;
    }
    else
    {
        output.setX(x);
        output.setY(y);
        isLatLon = false;
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
