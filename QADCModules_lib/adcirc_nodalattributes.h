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
 * \class adcirc_nodalattributes
 *
 * \brief Class that handles an ADCIRC fort.13 file
 *
 * This class is designed to handle functions regarding an
 * adcirc fort.13 nodal attributes file. Individual nodal
 * attributes are stored as adcirc_nodalparameter objects
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_NODALATTRIBUTES_H
#define ADCIRC_NODALATTRIBUTES_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QTextStream>
#include <QStringList>

#include "QADCModules_global.h"
#include "adcirc_nodalparameter.h"
#include "adcirc_errors.h"

class QADCMODULESSHARED_EXPORT adcirc_nodalattributes : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_nodalattributes(QObject *parent = 0);

    //...PUBLIC VARIABLES...//

    ///Filename of the file that will be read or was read
    QString filename;

    ///Title/header found at the beginning of the nodal attributes file
    QString title;

    ///Number of nodal attributes found within this file
    int numParameters;

    ///Number of nodes in the mesh that this nodal attribute files references
    int numNodes;

    ///Vector of objects containing the nodal parameters read from the file
    QVector<adcirc_nodalparameter*> nodalParameters;

    ///adcirc_errors class to describe any errors to the user
    adcirc_errors *error;


    //...PUBLIC FUNCTIONS...//

    int read(QString inputFilename);

    int write(QString outputFilename, bool userSpecifiedDefaultValues = false);

    int locateAttribute(QString attributeName);

protected:

    //...PROTECTED FUNCTIONS...//
    int readNodalAttributesFile();

    int writeNodalAttributesFile(QString outputFilename, bool userSpecifiedDefaultValues);

private:
    //...PRIVATE VARIABLES...//

    ///Mapping function between the name of a nodal parameter and its position in the nodalParameters vector
    QMap<QString,int> attributeLocations;

};

#endif // ADCIRC_NODALATTRIBUTES_H
