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
 * \class QADCModules_errors
 *
 * \brief Class to relay error messages to the user
 *
 * \author Zach Cobell
 *
 * The QADCModules_errors class is designed to be used by all other classes
 * in this suite of classes to relay error messages. Error messages are
 * mapped to a set of descriptions on startup so that the user can easily
 * understand where a particular function has failed. All routines must
 * return an error code as defined by this class or no error (ERROR_NOERROR)
 * to achieve successful completion
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef QADCMODULES_ERRORS_H
#define QADCMODULES_ERRORS_H

#include "QADCModules_global.h"
#include <QObject>
#include <QMap>

//----------------------------------------------------//
//...Define a set of errors for use with this
//   class


//...GENERIC ERRORS
#define ERROR_NOERROR      -999900000
#define ERROR_FILEOPENERR  -999901001
#define ERROR_NULLFILENAM  -999901002
#define ERROR_FILENOEXIST  -999901003


//...ADCIRC_MESH CLASS ERRORS
#define ERROR_MESHREAD_HEADER        -999902001
#define ERROR_MESHREAD_NODERR        -999902002
#define ERROR_MESHREAD_ELEMER        -999902003
#define ERROR_MESHREAD_BNDERR        -999902004
#define ERROR_MESHREAD_NODNUM        -999902005
#define ERROR_MESHREAD_ELENUM        -999902006
#define ERROR_MESHREAD_BNDUNK        -999902007
#define ERROR_MESHREAD_BNDNUL        -999902008
#define ERROR_MESHREAD_UNEXPECTEDEND -999902009
#define ERROR_LEVEE_BELOWTOPO        -999902010
#define ERROR_DISJOINTNODES          -999902011
#define ERROR_OVERLAPPINGELEMENTS    -999902012
#define ERROR_OVERLAPPINGBOUNDARIES  -999902013
#define ERROR_DUPLICATEELEMENTS      -999902014


//...ADCIRC_NODALPARAM CLASS ERRORS
#define ERROR_NODALPARAM_DEFAULTVALUESIZE -999903001
#define ERROR_NODALPARAM_NOTENOUGHVALUES  -999903002
#define ERROR_NODALPARAM_READERROR        -999903003
#define ERROR_NODALPARAM_OUTOFRANGE       -999903004


//...ADCIRC_NODALATTRIBUTES CLASS ERRORS
#define ERROR_NODALATT_HEADERR       -999904001
#define ERROR_NODALATT_BODYERR       -999904002
#define ERROR_NODALATT_UNEXPECTEDEND -999904003


//...PROJ4 CLASS ERRORS
#define ERROR_PROJ4_CANNOTREADEPSGFILE -999905001
#define ERROR_PROJ4_NOSUCHPROJECTION   -999905002
#define ERROR_PROJ4_INTERNAL           -999905003


//...QKDTREE2 CLASS ERRORS
#define ERROR_QKDTREE2_SIZEMISMATCH    -999906001

//----------------------------------------------------//


class QADCMODULESSHARED_EXPORT QADCModules_errors : public QObject
{
    Q_OBJECT
public:

    explicit QADCModules_errors(QObject *parent = 0);

    int resetError();

    int setError(int newErrorCode);

    int getError();

    QString getErrorString();

protected:

    int initializeErrors();

private:
    ///Stores the current error code for retrieval later
    int errorCode;

    ///Map function between error codes and their descriptions
    QMap<int,QString> errorMapping;


};

#endif // QADCMODULES_ERRORS_H
