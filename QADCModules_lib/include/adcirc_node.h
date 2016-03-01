/*-------------------------------GPL-------------------------------------//
//
// QADCModules - A library for working with ADCIRC models
// Copyright (C) 2015  Zach Cobell
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
 * \class adcirc_node
 *
 * \brief Class that describes an ADCIRC node
 *
 * This class describes a single adcirc node. Functions are provided
 * to read/write a node in the format used in an ADCIRC mesh file
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_NODE_H
#define ADCIRC_NODE_H

#include <QObject>
#include <QStringList>

#include "QADCModules_global.h"

class QADCMODULESSHARED_EXPORT adcirc_node : public QObject
{
    Q_OBJECT
public:

    explicit adcirc_node(QObject *parent = 0);

    ///X position of the ADCIRC node
    double x;

    ///Y position of the ADCIRC node
    double y;

    ///Z elevation of the ADCIRC node (positive depth)
    double z;

    ///ID for the ADCIRC node
    int    id;


    //...PUBLIC FUNCTIONS...//

    int fromString(QString line);

    QString toString(bool gcs = true);

};

//...OPERATORS...//
bool operator<(const adcirc_node n1,const adcirc_node n2);
bool operator>(const adcirc_node n1,const adcirc_node n2);

#endif // ADCIRC_NODE_H
