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
/**
 * \class adcirc_output_record
 *
 * \brief Class to handle a single ADCIRC global output record
 *
 * \author Zach Cobell
 *
 * Class to handle a single ADCIRC global output record
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_OUTPUT_RECORD_H
#define ADCIRC_OUTPUT_RECORD_H

#include <QObject>
#include <QVector>

#include "QADCModules_global.h"
#include "QADCModules_errors.h"

class QADCMODULESSHARED_EXPORT adcirc_output_record : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_output_record(int numNodes, QObject *parent = 0);

    //...PUBLIC VARIABLES...//

    ///Number of nodes in the output records
    int numNodes;

    ///Model time (seconds) when this record was written
    qreal modelTime;

    ///Model time step
    int modelStep;

    ///Vector containig the model data for this output record
    QVector<qreal> scalar;

    ///Vector containing the model data for this output record (u-component of the vector)
    QVector<qreal> vector_u;

    ///Vector containing the model data for this output record (v-component of the vector)
    QVector<qreal> vector_v;

    //...PUBLIC FUNCTIONS...//
    int toMagnitude();

};

#endif // ADCIRC_OUTPUT_RECORD_H
