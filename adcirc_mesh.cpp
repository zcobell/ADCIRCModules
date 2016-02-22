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
#include "adcirc_mesh.h"
#include "QADCModules_flags.h"

adcirc_mesh::adcirc_mesh(QObject *parent) : QObject(parent)
{

}

//-----------------------------------------------------------------------------------------//
//
//
//   P U B L I C
//             F U N C T I O N S
//
//
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::read()
{
    //...Check for a null string
    if(this->filename==QString())
    {
        this->errorCode = QADCMODULES_NULLFILENAM;
        return this->errorCode;
    }

    //...Check for file exists
    QFile thisFile(this->filename);
    if(!thisFile.exists())
    {
        this->errorCode = QADCMODULES_FILENOEXIST;
        return this->errorCode;
    }

    //...Assuming these two checks passed, we can call
    //   the main routine
    int ierr = this->readMesh();

    return ierr;
}


int adcirc_mesh::write(QString outputFile)
{
    if(outputFile==QString())
    {
        this->errorCode = QADCMODULES_NULLFILENAM;
        return this->errorCode;
    }

    //...Assuming the filename is valid, write the mesh
    //int ierr = this->writeMesh();

    return 0;
}



//-----------------------------------------------------------------------------------------//
//
//
//   P R I V A T E
//             F U N C T I O N S
//
//
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readMesh()
{

    QFile       meshFile(this->filename);
    QString     meshStringData,tempString;
    QStringList meshFileList,tempList;
    QByteArray  meshByteData;
    int         i,ierr;
    bool        err;

    if(!meshFile.open(QIODevice::ReadOnly))
    {
        this->errorCode = QADCMODULES_FILEOPENERR;
        return this->errorCode;
    }

    //...Read the entire mesh file at once
    meshByteData   = meshFile.readAll();
    meshStringData = QString(meshByteData);
    meshFileList   = meshStringData.split("\n");
    meshByteData   = QByteArray();
    meshStringData = QString();
    meshFile.close();

    //...Grab the header
    this->title = meshFileList.value(0);

    //...Grab the number of elements,nodes
    tempString = meshFileList.value(1);
    tempList   = tempString.simplified().split(" ");
    tempString = tempList.value(0);
    this->numNodes = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = QADCMODULES_MESHREAD_HEADER;
        return this->errorCode;
    }
    tempString = tempList.value(1);
    this->numElements = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = QADCMODULES_MESHREAD_HEADER;
        return this->errorCode;
    }

    //...Allocate the nodes
    this->allocateNodes(this->numNodes);

    //...Allocate the elements
    this->allocateElements(this->numElements);

    //...Loop over the nodes
    for(i=0;i<this->numNodes;i++)
    {
        ierr = this->readNode(meshFileList.value(i+2),i,this->nodes[i]);
        if(ierr!=0)
        {
            this->errorCode = ierr;
            return ierr;
        }
    }

    //...Loop over the elements
    for(i=0;i<this->numElements;i++)
    {
        ierr = this->readElement(meshFileList.value(this->numNodes+2+i),i,this->elements[i]);
        if(ierr!=0)
        {
            this->errorCode = ierr;
            return ierr;
        }
    }

    return 0;
}

int adcirc_mesh::allocateNodes(int n)
{
    this->nodes.resize(n);
    for(int i=0;i<n;i++)
        this->nodes[i] = new adcirc_node(this);
    return 0;
}

int adcirc_mesh::allocateElements(int n)
{
    this->elements.resize(n);
    for(int i=0;i<n;i++)
        this->elements[i] = new adcirc_element(this);
    return 0;
}


int adcirc_mesh::readNode(QString line, int index, adcirc_node *node)
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
    node->id = tempInt;

    tempString = tempList.value(1);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return QADCMODULES_MESHREAD_NODERR;
    node->x = tempDouble;
    tempString = tempList.value(2);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return QADCMODULES_MESHREAD_NODERR;
    node->y = tempDouble;
    tempString = tempList.value(3);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return QADCMODULES_MESHREAD_NODERR;
    node->z = tempDouble;

    return 0;
}

int adcirc_mesh::readElement(QString line, int index, adcirc_element *element)
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

    element->id = tempInt;

    tempString = tempList.value(2);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_ELEMER;
    element->connections[0] = tempInt;

    tempString = tempList.value(3);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_ELEMER;
    element->connections[1] = tempInt;

    tempString = tempList.value(4);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return QADCMODULES_MESHREAD_ELEMER;
    element->connections[2] = tempInt;

    return 0;
}
