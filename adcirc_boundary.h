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
 * \addtogroup QADCModules
 *
 * \brief Class that describes an ADCIRC boundary
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
#ifndef ADCIRC_BOUNDARY_H
#define ADCIRC_BOUNDARY_H

#include <QADCModules_flags.h>
#include <QObject>
#include <QVector>

class adcirc_boundary : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_boundary(QObject *parent = 0);

    int             numNodes;       ///Number of nodes on the boundary string
    int             code;           ///ADCIRC model boundary code
    QVector<int>    n1;             ///First node in the boundary
    QVector<int>    n2;             ///Second node in the boundary. Used for weirs where boundary conditions have an opposite side
    QVector<double> crest;          ///Boundary crest. Used for weir type boundaries
    QVector<double> supercritical;  ///Coefficient of supercritical flow over a weir
    QVector<double> subcritical;    ///Coefficient of subcritical flow over a weir
    QVector<double> pipeHeight;     ///Height of the pipe/culvert in a weir
    QVector<double> pipeDiam;       ///Diameter of the pipe/culvert in a weir
    QVector<double> pipeCoef;       ///Pipe coefficient for the pipe/culvert in a weir

signals:

public slots:
};

#endif // ADCIRC_BOUNDARY_H
