#include <QCoreApplication>
#include <QPointer>
#include <QDebug>
#include "QADCModules.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QPointer<adcirc_mesh> thisMesh = new adcirc_mesh();

    int ierr = thisMesh->read("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv.grd");
    qDebug() << thisMesh->error->errorCode;
    qDebug() << thisMesh->error->getErrorString();

    qDebug() << thisMesh->write("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv-me.grd");

    return ierr;
}
