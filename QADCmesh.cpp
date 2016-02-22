#include "QADCmesh.h"

QADCmesh::QADCmesh(QObject *parent) : QObject(parent)
{

}

int QADCmesh::readADCIRCMesh(QString filename, adcirc_mesh &myMesh)
{

    QFile       meshFile(filename);
    QString     meshStringData,tempString;
    QStringList meshFileList,tempList;
    QByteArray  meshByteData;
    int         i,ierr;
    bool        err;

    if(!meshFile.open(QIODevice::ReadOnly))
        return QADCMODULES_FILEOPENERR;

    //...Read the entire mesh file at once
    meshByteData   = meshFile.readAll();
    meshStringData = QString(meshByteData);
    meshFileList   = meshStringData.split("\n");
    meshByteData   = QByteArray();
    meshStringData = QString();
    meshFile.close();

    //...Grab the header
    myMesh.title = meshFileList.value(0);

    //...Grab the number of elements,nodes
    tempString = meshFileList.value(1);
    tempList   = tempString.simplified().split(" ");
    tempString = tempList.value(0);
    myMesh.numNodes = tempString.toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_HEADER;
    tempString = tempList.value(1);
    myMesh.numElements = tempString.toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_HEADER;

    //...Allocate the vectors
    myMesh.nodes.resize(myMesh.numNodes);
    myMesh.elements.resize(myMesh.numElements);

    //...Loop over the nodes
    for(i=0;i<myMesh.numNodes;i++)
    {
        ierr = this->readADCIRCNode(meshFileList.value(i+2),i,myMesh.nodes[i]);
        if(ierr!=0)
        {
            this->errorCode = ierr;
            return ierr;
        }
    }

    //...Loop over the elements
    for(i=0;i<myMesh.numElements;i++)
    {
        ierr = this->readADCIRCElement(meshFileList.value(myMesh.numNodes+2+i),i,myMesh.elements[i]);
        if(ierr!=0)
        {
            this->errorCode = ierr;
            return ierr;
        }
    }

    return 0;
}


int QADCmesh::readADCIRCNode(QString line, int index, adcirc_node &node)
{
    QStringList tempList;
    QString     tempString;
    int         tempInt;
    double      tempDouble;
    bool        err;

    tempList   = line.simplified().split(" ");
    tempString = tempList.value(0);
    tempInt    = tempString.toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_NODERR;
    if(tempInt!=index+1)
        return QADCMODULES_MESHREAD_NODNUM;
    node.id = tempInt;

    tempString = tempList.value(1);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return QADCMODULES_MESHREAD_NODERR;
    node.x = tempDouble;
    tempString = tempList.value(2);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return QADCMODULES_MESHREAD_NODERR;
    node.y = tempDouble;
    tempString = tempList.value(3);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return QADCMODULES_MESHREAD_NODERR;
    node.z = tempDouble;

    return 0;
}

int QADCmesh::readADCIRCElement(QString line, int index, adcirc_element &element)
{
    QStringList tempList;
    QString     tempString;
    int         tempInt;
    bool        err;

    tempList   = line.simplified().split(" ");
    tempString = tempList.value(0);
    tempInt    = tempString.toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_ELEMER;
    if(tempInt!=index+1)
        return QADCMODULES_MESHREAD_ELENUM;

    element.id = tempInt;
    element.numConnections = 3;

    tempString = tempList.value(2);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_ELEMER;
    element.node1 = tempInt;

    tempString = tempList.value(3);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_ELEMER;
    element.node2 = tempInt;

    tempString = tempList.value(4);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_ELEMER;
    element.node3 = tempInt;

    return 0;
}
