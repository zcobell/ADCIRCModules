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
 * \class adcirc_global_output
 *
 * \brief Class that describes an ADCIRC global output file
 *
 * \author Zach Cobell
 *
 * Class that describes an ADCIRC global output file
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_OUTPUT_H
#define ADCIRC_OUTPUT_H

#include <QObject>
#include <QSharedPointer>

#include "QADCModules_global.h"
#include "adcirc_mesh.h"
#include "adcirc_output_record.h"
#include "QADCModules_errors.h"

class QADCMODULESSHARED_EXPORT adcirc_global_output : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_global_output(QString filename, QObject *parent = 0);

    ///filename of the global ADCIRC output file to read
    QString filename;

    ///Number of nodes in the global ADCIRC output file
    int numNodes;

    ///Number of columns (i.e. scalar or vector data) for ths ADCIRC output file
    int numColumns;

    ///For netCDF files, the mesh information is included in the file. It will be stored here
    adcirc_mesh *mesh;

    ///Variable containing the error codes returned by the functions within this class
    QADCModules_errors *error;

    ///Output data from the ADCIRC output file. Note that only one record is here because
    /// large models can easily overwealm the available memory. Consecutive reads of the file
    /// will overwrite the data in this variable saving memory.
    adcirc_output_record* outputData;

    //...Public Functions...//
    int read(int record);

    int write(int record, QString outputFilename);

protected:

    //...Protected Functions...//

    int initializeNetcdfVariables();

    int findNetCDFVariable(int &ncid, int &numVariables, int &variable1, int &variable2);

    int readAdcircGlobalOutputNetCDF(int record);

    int readAdcircGlobalOutputAscii();

private:

    //...Private Variables...//

    ///Variable describing if the adcirc_mesh was read from the netCDF file
    bool isMeshInitialized;

    ///Read file descriptor
    QFile readFid;

    ///Write file descriptor
    QFile writeFid;

    ///Variable containing the netCDF variable types found in adcirc output files.
    QStringList netCDFVariables;

};

#endif // ADCIRC_OUTPUT_H
