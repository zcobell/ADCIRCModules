#include <QCoreApplication>
#include <QPointer>
#include <QDebug>
#include "QADCModules.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QPointer<QADCModules> thisMod = new QADCModules();
    QPointer<adcirc_mesh> thisMesh = new adcirc_mesh();

    thisMesh->filename = "C:/Users/zcobell/Documents/Codes/QADCModules/QADCModules_Testing/ms-riv.grd";
    int ierr = thisMesh->read();
    qDebug() << thisMesh->getErrorCode();
    qDebug() << thisMesh->getErrorString();

    return ierr;
}
