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
#include <QStringList>
#include <QFile>

#include "QADCModules_global.h"
#include "adcirc_forward.h"

class QADCMODULESSHARED_EXPORT adcirc_global_output : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_global_output(QString filename, QObject *parent = 0);

    explicit adcirc_global_output(QString filename, bool ignoreMesh, QObject *parent = 0);

    ~adcirc_global_output();

    ///filename of the global ADCIRC output file to read
    QString filename;

    ///Title of the file found in the first line of an ascii output file
    QString title;

    ///Number of nodes in the global ADCIRC output file
    int numNodes;

    ///Number of columns (i.e. scalar or vector data) for ths ADCIRC output file
    int numColumns;

    ///Total number of output records in the file
    int numRecords;

    ///Number of model time steps between consecutive records in the output file
    int timestepsBetweenSnaps;

    ///Time (seconds) between consecutive records in the output file
    qreal timeBetweenSnaps;

    ///Default value (or _FillValue in netCDF)
    qreal defaultValue;

    ///For netCDF files, the mesh information is included in the file. It will be stored here
    adcirc_mesh *mesh;

    ///Variable containing the error codes returned by the functions within this class
    QADCModules_errors *error;

    ///Output data from the ADCIRC output file. Note that only one record is here because
    /// large models can easily overwealm the available memory. Consecutive reads of the file
    /// will overwrite the data in this variable saving memory.
    adcirc_output_record* outputData;

    //...Public Functions...//
    int readNetCDF(int record);

    int readAscii();

    int readNextAscii();

    //int writeNetCDF(int record, QString outputFilename);

    //int writeAscii();

    int toShapefile(QString outputFile);

    int setMesh(adcirc_mesh *mesh);

protected:

    //...Protected Functions...//

    int initializeNetcdfVariables();

    int findNetCDFVariables(int &ncid, QVector<int> &varIDs);

    int readAdcircGlobalOutputNetCDF(int record);

    int readAdcircGlobalOutputAsciiHeader();

    int readNextAdcircGlobalOutputAscii();

private:

    //...Private Variables...//

    ///Variable that allows the user to skip reading of the internal mesh
    ///inside the adcirc_global_output file
    bool dontReadMesh;

    ///Variable that holds the index of the last record read from the file
    int lastRecordRead;

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
