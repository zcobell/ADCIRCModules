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
 * \class adcirc_element
 *
 * \brief Class that describes an ADCIRC triangular element
 *
 * \author Zachary Cobell
 * \version Version 0.1
 * \date 02/21/2016
 *
 * Contact: zcobell@gmail.com
 *
 * Created on: 02/21/2016
 *
 */
#ifndef ADCIRC_ELEMENT_H
#define ADCIRC_ELEMENT_H

#include <QObject>
#include <QVector>
#include "QADCModules_global.h"
#include "adcirc_node.h"

class QADCMODULESSHARED_EXPORT adcirc_element : public QObject
{
    Q_OBJECT
public:

    /** \brief Constructor
     *  Takes QObject reference as input
     * @param *parent [in] reference to QObject. Enables automatic memory management to avoid memory leaks
     **/
    explicit adcirc_element(QObject *parent = 0);

    ///The ID of the element
    int id;

    ///The number of nodes in this element. Note that only 3 connections are used for ADCIRC meshes
    int numConnections;

    ///Vector containing the list of nodes that make up this element as pointers to the node array
    QVector<adcirc_node*> connections;

};

#endif // ADCIRC_ELEMENT_H
