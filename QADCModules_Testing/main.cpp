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
    ierr = thisMesh->read("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv.grd");
    qDebug() << thisMesh->error->errorCode;
    qDebug() << thisMesh->error->getErrorString();

    qDebug() << "Writing mesh test...";
    ierr = thisMesh->write("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv-me.grd");
    qDebug() << thisMesh->error->errorCode;
    qDebug() << thisMesh->error->getErrorString();

    //...Fort13 Read/Write Test
    qDebug() << "Reading Fort.13 test...";
    QPointer<adcirc_nodalattributes> thisNodalParam = new adcirc_nodalattributes();
    ierr = thisNodalParam->read("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv.13");
    qDebug() << thisNodalParam->error->errorCode;
    qDebug() << thisNodalParam->error->getErrorString();

    return 0;
}
