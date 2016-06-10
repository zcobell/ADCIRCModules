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
 * \class adcirc_nodalattribute
 *
 * \brief Class that handles the data from an ADCIRC fort.13 file
 *
 * This class is designed to handle individual nodal data that is read
 * in from an ADCIRC fort.13 file
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_NODALATTRIBUTE_H
#define ADCIRC_NODALATTRIBUTE_H

#include <QObject>
#include <QVector>

#include "QADCModules_global.h"
#include "adcirc_forward.h"

class QADCMODULESSHARED_EXPORT adcirc_nodalattribute : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_nodalattribute(adcirc_nodalparameter *nodalMetadata, QObject *parent = 0);

    ///Value(s) for nodal parameter at this node
    QVector<qreal> values;

    ///Metadata for nodal parameters
    adcirc_nodalparameter *metadata;

};

#endif // ADCIRC_NODALATTRIBUTE_H
