#ifndef ADCIRC_MESH_H
#define ADCIRC_MESH_H
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
 * \class adcirc_mesh
 *
 * \addtogroup QADCModules
 *
 * \brief Class that describes an ADCIRC mesh
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
#include <QObject>
#include <QVector>
#include <QFile>
#include "adcirc_node.h"
#include "adcirc_element.h"
#include "adcirc_boundary.h"

class adcirc_mesh : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_mesh(QObject *parent = 0);

    //...Public variables
    QString                            filename;
    QString                            title;
    int                                numElements;
    int                                numNodes;
    QVector<adcirc_node*>              nodes;
    QVector<adcirc_element*>           elements;
    int                                numOpenBoundaries;
    int                                totNumOpenBoundaryNodes;
    int                                numLandBonudaries;
    int                                totNumLandBoundaryNodes;
    QVector<adcirc_boundary*>          oceanBC;
    QVector<adcirc_boundary*>          landBC;

    //...Public functions
    int read();
    int write(QString outputFile);

signals:

public slots:

private:

    //...PRIVATE VARIABLES...//
    int errorCode;
    QString errorString;

    //...PRIVATE FUNCTIONS...//
    int readMesh();
    int allocateNodes(int n);
    int allocateElements(int n);
    int readNode(QString line, int index, adcirc_node *node);
    int readElement(QString line, int index, adcirc_element *element);

};

#endif // ADCIRC_MESH_H
