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

#include <QCoreApplication>
#include <QPointer>
#include <QDebug>
#include "QADCModules.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int ierr;

    ierr = 0;

    //...Mesh Read/Write Test
    QPointer<adcirc_mesh> thisMesh = new adcirc_mesh();

    qDebug() << "Reading mesh test...";
    ierr = thisMesh->read("../../QADCModules/tests/test_files/ms-riv.grd");
    qDebug() << thisMesh->error->errorCode;
    qDebug() << thisMesh->error->getErrorString();

    if(thisMesh->error->errorCode!=ERROR_NOERROR)
    {
        qDebug() << "Mesh read test failed.";
        return ERROR_NOERROR;
    }

    qDebug() << "Writing mesh test...";
    ierr = thisMesh->write("../../QADCModules/tests/test_files/ms-riv-me.grd");
    qDebug() << thisMesh->error->errorCode;
    qDebug() << thisMesh->error->getErrorString();

    if(thisMesh->error->errorCode!=ERROR_NOERROR)
    {
        qDebug() << "Mesh write test failed.";
        return ERROR_NOERROR;
    }

    qDebug() << "Renumbering mesh test...";
    ierr = thisMesh->renumber();
    ierr = thisMesh->write("../../QADCModules/tests/test_files/ms-riv-me-renumber.grd");
    qDebug() << thisMesh->error->errorCode;
    qDebug() << thisMesh->error->getErrorString();

    //...Fort13 Read/Write Test
    qDebug() << "Reading Fort.13 test...";
    QPointer<adcirc_nodalattributes> thisNodalParam = new adcirc_nodalattributes();
    ierr = thisNodalParam->read("../../QADCModules/tests/test_files/test_files/ms-riv.13");
    qDebug() << thisNodalParam->error->errorCode;
    qDebug() << thisNodalParam->error->getErrorString();

    if(thisNodalParam->error->errorCode!=ERROR_NOERROR)
    {
        qDebug() << "Fort13 read test failed.";
        return ERROR_NOERROR;
    }

    qDebug() << "Writing fort.13 test...";
    qDebug() << thisNodalParam->write("../../QADCModules/tests/test_files/ms-riv-me.13");
    qDebug() << thisNodalParam->error->getErrorString();

    if(thisNodalParam->error->errorCode!=ERROR_NOERROR)
    {
        qDebug() << "Fort13 write test failed.";
        return ERROR_NOERROR;
    }

    return 0;
}
