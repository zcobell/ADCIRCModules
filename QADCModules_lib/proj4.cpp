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
#include "QADCModules_errors.h"
#include "proj4.h"
#include "proj_api.h"
#include "epsg.h"

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for the proj4 wrapper class
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

}
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
// Function to execute a coordinate system transformation using Proj4
//-----------------------------------------------------------------------------------------//
/** \brief Function to execute a coordinate system transformation using Proj4
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
    qreal x,y,z;
    int ierr;

    ierr = 0;
    z = 0.0;

    if(!epsgMapping.contains(inputEPSG))
        return ERROR_PROJ4_NOSUCHPROJECTION;

    if(!epsgMapping.contains(outputEPSG))
        return ERROR_PROJ4_NOSUCHPROJECTION;

    QString currentInitialization = epsgMapping[inputEPSG];
    QString outputInitialization  = epsgMapping[outputEPSG];

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


//-----------------------------------------------------------------------------------------//
// Function to execute a coordinate system transformation using Proj4
//-----------------------------------------------------------------------------------------//
/** \brief Function to execute a coordinate system transformation using Proj4
 *
 * @param[in]  inputEPSG  EPSG that coordinates are currently in
 * @param[in]  outputEPSG EPSG that the coordinates will be converted to
 * @param[in]  input      QVector of QPointF objects containing the locations to be converted
 * @param[out] output     QVector of QPointF objects containing the converted coordiantes
 * @param[out] isLatLon   Bool that determine if the coordinates are lat/lon or otherwise
 *
 * Function to execute a coordinate system transformation using Proj4
 *
 **/
//-----------------------------------------------------------------------------------------//
int proj4::transform(int inputEPSG, int outputEPSG, QVector<QPointF> &input, QVector<QPointF> &output, bool &isLatLon)
{
    projPJ inputPJ,outputPJ;
    QVector<qreal> x,y,z;
    int i,ierr;

    ierr = 0;

    if(!epsgMapping.contains(inputEPSG))
        return ERROR_PROJ4_NOSUCHPROJECTION;

    if(!epsgMapping.contains(outputEPSG))
        return ERROR_PROJ4_NOSUCHPROJECTION;

    QString currentInitialization = epsgMapping[inputEPSG];
    QString outputInitialization  = epsgMapping[outputEPSG];

    if(!(inputPJ = pj_init_plus(currentInitialization.toLatin1().data())))
        return ERROR_PROJ4_INTERNAL;

    if(!(outputPJ = pj_init_plus(outputInitialization.toLatin1().data())))
        return ERROR_PROJ4_INTERNAL;

    x.resize(input.size());
    y.resize(input.size());
    z.resize(input.size());
    z.fill(0.0);
    output.resize(input.size());

    for(i=0;i<input.size();i++)
    {
        if(pj_is_latlong(inputPJ))
        {
            x[i] = input[i].x()*DEG_TO_RAD;
            y[i] = input[i].y()*DEG_TO_RAD;
        }
        else
        {
            x[i] = input[i].x();
            y[i] = input[i].y();
        }

        ierr = pj_transform(inputPJ,outputPJ,1,1,&x[i],&y[i],&z[i]);

        if(ierr!=0)
            return ERROR_PROJ4_INTERNAL;

        if(pj_is_latlong(outputPJ))
        {
            output[i].setX(x[i]*RAD_TO_DEG);
            output[i].setY(y[i]*RAD_TO_DEG);
        }
        else
        {
            output[i].setX(x[i]);
            output[i].setY(y[i]);
        }
    }

    if(pj_is_latlong(outputPJ))
        isLatLon = true;
    else
        isLatLon = false;

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
