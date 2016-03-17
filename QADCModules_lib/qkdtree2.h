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
 * \class qKdtree2
 *
 * \brief Class that serves as an interface to the kdtree2 library
 *
 * This function serves as the C++ interface to the kdtree2 library. Kdtree2
 * requires the boost library headers that are included with this code
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef QKDTREE2_H
#define QKDTREE2_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>

#include "QADCModules_global.h"
#include "adcirc_errors.h"
#include "kdtree2.hpp"

class QADCMODULESSHARED_EXPORT qKdtree2 : public QObject
{
    Q_OBJECT
public:
    explicit qKdtree2(QObject *parent = 0);

    ///Variable holding the total number of points in the search tree
    int numDataPoints;

    ///Variable that ensures the search tree is initialized
    bool initialized;

    int build(QVector<double> &x, QVector<double> &y);

    int findNearest(double x, double y, int &index);
    int findNearest(QPointF location, int &index);

    int findXNearest(double x, double y, int nn, QVector<int> &indicies);
    int findXNearest(QPointF location, int nn, QVector<int> &indicies);

private:

    ///Pointer to variable holding the kdtree search tree
    QSharedPointer<kdtree2> tree;

};

#endif // QKDTREE2_H
