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
 * \class adcirc_boundary
 *
 * \brief Class that describes an ADCIRC boundary
 *
 * This class handles describing ADCIRC boundary conditions. Functions
 * are provided to read and write strings of boundary nodes
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_BOUNDARY_H
#define ADCIRC_BOUNDARY_H

#include <QObject>
#include <QVector>
#include <QStringList>
#include <QMap>

#include "QADCModules_global.h"
#include "adcirc_node.h"


class QADCMODULESSHARED_EXPORT adcirc_boundary : public QObject
{
    Q_OBJECT
public:
    /** \brief Constructor
     *  Constructor for adcirc_boundary. Takes QObject reference as input
     * @param *parent [in] reference to QObject. Enables automatic memory management to avoid memory leaks
     **/
    explicit adcirc_boundary(QObject *parent = 0);

    ///Number of nodes on the boundary string
    int             numNodes;

     ///ADCIRC model boundary code
    int             code;

    ///First node in the boundary. Pointer to the node vector.
    QVector<adcirc_node*>    n1;

    ///Second node in the boundary. Used for weirs where boundary conditions have an opposite side. Pointer to the node vector.
    QVector<adcirc_node*>    n2;

    ///Boundary crest. Used for weir type boundaries
    QVector<double> crest;

    ///Coefficient of supercritical flow over a weir
    QVector<double> supercritical;

    ///Coefficient of subcritical flow over a weir
    QVector<double> subcritical;

    ///Height of the pipe/culvert in a weir
    QVector<double> pipeHeight;

    ///Diameter of the pipe/culvert in a weir
    QVector<double> pipeDiam;

    ///Pipe coefficient for the pipe/culvert in a weir
    QVector<double> pipeCoef;

    //...PUBLIC FUNCTIONS...//

    int fromString(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping);

    QStringList toStringList(bool isOpenBC = false);

    int setupBoundary(int code, int size);

protected:

    int readBoundarySingleNode(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping);

    int readBoundaryOneSidedWeir(QString data, int index,QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping);

    int readBoundaryTwoSidedWeir(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping);

    int readBoundaryCrossBarrierPipe(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping);

};

#endif // ADCIRC_BOUNDARY_H
