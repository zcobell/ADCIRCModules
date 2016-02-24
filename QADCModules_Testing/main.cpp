#include <QCoreApplication>
#include <QPointer>
#include <QDebug>
#include "QADCModules.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QPointer<adcirc_mesh> thisMesh = new adcirc_mesh();

    int ierr = thisMesh->read("C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv.grd");
    qDebug() << thisMesh->getErrorCode();
    qDebug() << thisMesh->getErrorString();

    return ierr;
}
