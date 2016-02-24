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
 *
 * \class QADCModules
 *
 * \brief A Qt C++ package for use with the ADCIRC model
 *
 * This class provides basic functions for working with
 * ADCIRC model data. (http://www.adcirc.org) It is written
 * in Qt C++ and has been tested with Qt version 5.5. The package
 * is provided as a dynamicly linked library (Windows)
 * or shared object file (Unix)
 *
 * \author Zachary Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef QADCMODULES_H
#define QADCMODULES_H

#include <QObject>
#include <QVector>
#include <QFile>
#include <QMap>

#include "QADCModules_global.h"
#include "adcirc_mesh.h"


class QADCMODULESSHARED_EXPORT QADCModules : public QObject
{
    Q_OBJECT
public:

    /** \brief Constructor
     *  Takes QObject reference as input
     * @param *parent [in] reference to QObject. Enables automatic memory management to avoid memory leaks
     **/
    explicit QADCModules(QObject *parent = 0);

    ~QADCModules();


};

#endif // QARCADISUTIL_H
