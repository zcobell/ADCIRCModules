#include "QADCModules.h"
#include "QADCModules_flags.h"


QADCModules::QADCModules(QObject *parent) : QObject(parent)
{
    int ierr = initialize_errors();
}

QADCModules::~QADCModules()
{

}

int QADCModules::initialize_errors()
{
    this->errorMapping[QADCMODULES_FILEOPENERR]     = "The specified file could not be correctly opened.";
    this->errorMapping[QADCMODULES_MESHREAD_NODERR] = "There was an error while reading the nodes from the mesh file.";
    this->errorMapping[QADCMODULES_MESHREAD_ELEMER] = "There was an error while reading the elements from the mesh file.";
    this->errorMapping[QADCMODULES_MESHREAD_BNDERR] = "There was an error while reading the boundary segments from the mesh file.";
    this->errorMapping[QADCMODULES_MESHREAD_NODNUM] = "The nodes in the mesh are not sequentially numbered.";
    this->errorMapping[QADCMODULES_MESHREAD_ELENUM] = "The elements in the mesh are not sequantially numbered.";

    return 0;
}

