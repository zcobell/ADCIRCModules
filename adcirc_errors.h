#ifndef ADCIRC_ERRORS_H
#define ADCIRC_ERRORS_H

#include "QADCModules_global.h"
#include <QObject>
#include <QMap>

//----------------------------------------------------//
//...Define a set of errors for use with this
//   class
#define ADCMESH_NOERROR         -999900000
#define ADCMESH_FILEOPENERR     -999901001
#define ADCMESH_NULLFILENAM     -999901002
#define ADCMESH_FILENOEXIST     -999901003
#define ADCMESH_MESHREAD_HEADER -999902001
#define ADCMESH_MESHREAD_NODERR -999902002
#define ADCMESH_MESHREAD_ELEMER -999902003
#define ADCMESH_MESHREAD_BNDERR -999902004
#define ADCMESH_MESHREAD_NODNUM -999902005
#define ADCMESH_MESHREAD_ELENUM -999902006
#define ADCMESH_MESHREAD_BNDUNK -999902007
#define ADCMESH_MESHREAD_BNDNUL -999902008
//----------------------------------------------------//


class QADCMODULESSHARED_EXPORT adcirc_errors : public QObject
{
    Q_OBJECT
public:

    explicit adcirc_errors(QObject *parent = 0);

    int errorCode;

    QString getErrorString();

protected:

    ///Map function between error codes and their descriptions
    QMap<int,QString> errorMapping;

    int initializeErrors();


};

#endif // ADCIRC_ERRORS_H
