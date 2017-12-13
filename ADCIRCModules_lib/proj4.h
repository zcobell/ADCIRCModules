/*-------------------------------GPL-------------------------------------//
//
// adcircmodules - A library for working with ADCIRC models
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
 * \class proj4
 *
 * \brief Class that serves as an interface to the standard C PROJ4 library
 *
 * This function serves as the C++ interface to the C library PROJ4 (which is
 * compiled into this code)
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef PROJ4_H
#define PROJ4_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QStringList>
#include <QPoint>

#include "adcirc_forward.h"

class proj4 : public QObject
{
    Q_OBJECT
public:
    explicit proj4(QObject *parent = 0);

    int readEPSGInitializations();

    int transform(int inputEPSG, int outputEPSG, QPointF &input, QPointF &output, bool &isLatLon);

    int transform(int inputEPSG, int outputEPSG, QVector<QPointF> &input, QVector<QPointF> &output, bool &isLatLon);

};

#endif // PROJ4_H
