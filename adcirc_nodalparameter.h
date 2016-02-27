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
 * \class adcirc_nodalparemeter
 *
 * \brief Class that handles a single nodal attribute from an
 *        ADCIRC fort.13 file
 *
 * This class is designed to handle individual nodal parameters that
 * are read from an ADCIRC fort.13 file
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_NODALPARAMETER_H
#define ADCIRC_NODALPARAMETER_H

#include <QObject>
#include <QVector>

#include "QADCModules_global.h"

class QADCMODULESSHARED_EXPORT adcirc_nodalparameter : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_nodalparameter(int numNodes, QString nodalAttributeName = "defaultName",
                                   QString nodalAttributeUnits = "none",
                                   int numValues = 1, QObject *parent = 0);

    //...PUBLIC VARIABLES...//

    ///Name of the nodal attribute
    QString name;

    ///Units for the nodal parameter
    QString units;

    ///Number of values associated with each node for this nodal attribute
    int nValues;

    ///Number of nodes in the mesh that this object is tied to
    int nNodes;

    ///Default value(s) to be used when writing the fort.13 file
    QVector<double> defaultValue;

    ///Values for the nodal parameter
    QVector< QVector<double> > values;

    //...PUBLIC FUNCTIONS...//
    int setDefaultValues(QVector<double> nodalParameterDefaultValues);

    int read(QStringList fileData);

    QStringList write();

protected:

};

#endif // ADCIRC_NODALPARAMETER_H
