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
#include "adcirc_output_record.h"
#include "QADCModules_errors.h"
#include <qmath.h>

adcirc_output_record::adcirc_output_record(int numNodes, QObject *parent) : QObject(parent)
{
    this->numNodes = numNodes;
}


//-----------------------------------------------------------------------------------------//
//...Function to read an ADCIRC mesh from netCDF
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_output_record::toMagnitude()
 * \brief This functions converts an ADCIRC output record with vector data to its scalar magnitude
 *
 * This functions converts an ADCIRC output record with vector data to its scalar magnitude
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_output_record::toMagnitude()
{
    if(this->numNodes != this->vector_u.size() || this->numNodes != this->vector_v.size())
        return ERROR_ADCIRCOUTPUT_SIZEMISMATCH;

    this->scalar.clear();

    this->scalar.resize(this->numNodes);

    for(int i=0;i<this->numNodes;i++)
        this->scalar[i] = qPow(qPow(this->vector_u[i],2) + qPow(this->vector_v[i],2),0.50);

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
