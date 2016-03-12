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
 * \class adcirc_node_table
 *
 * \brief Class that contains the list of elements around a node
 *
 * \author Zach Cobell
 *
 * Quite often, it is useful to have a list of the elements that are around
 * a specific node. This class is used to organize that information
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_NODE_TABLE_H
#define ADCIRC_NODE_TABLE_H

#include <QObject>
#include <QList>

#include "QADCModules_global.h"
#include "adcirc_node.h"
#include "adcirc_element.h"
#include "adcirc_errors.h"

class QADCMODULESSHARED_EXPORT adcirc_node_table : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_node_table(adcirc_node *node, QObject *parent = 0);

    ///Pointer to the actual adcirc_node object
    adcirc_node* node;

    ///Table of elements around the node
    QList<adcirc_element*> elementsAroundNode;

    //...PUBLIC FUNCTIONS...//
    int add(adcirc_element* addElement);

    int remove(adcirc_element* removeElement);

    int clear();

};

#endif // NODE_TABLE_H
