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
//    QPointer<adcirc_mesh> thisMesh = new adcirc_mesh();

//    ierr = thisMesh->read("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv.grd");
//    qDebug() << thisMesh->error->errorCode;
//    qDebug() << thisMesh->error->getErrorString();

//    qDebug() << thisMesh->write("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv-me.grd");

    //...Fort13 Read/Write Test
    QPointer<adcirc_nodalattributes> thisNodalParam = new adcirc_nodalattributes();
    thisNodalParam->read("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv.13");

    return 0;
}
