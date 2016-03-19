#ifndef ADCIRC_OUTPUT_H
#define ADCIRC_OUTPUT_H

#include <QObject>
#include <QSharedPointer>

#include "QADCModules_global.h"
#include "adcirc_mesh.h"
#include "adcirc_output_record.h"
#include "QADCModules_errors.h"
#include "netcdf_cpp.h"

class QADCMODULESSHARED_EXPORT adcirc_global_output : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_global_output(QString filename, QObject *parent = 0);

    QString filename;

    int numNodes;

    int numColumns;

    adcirc_mesh *mesh;

    QADCModules_errors *error;

    QVector<adcirc_output_record*> outputData;

    //...Public Functions...//
    int read(int record);

    int write(int record, QString outputFilename);


private:

    int initializeNetcdfVariables();

    int findNetCDFVariable(int &ncid, int &numVariables, int &variable1, int &variable2);

    int readAdcircGlobalOutputNetCDF(int record);

    int readAdcircGlobalOutputAscii();

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
