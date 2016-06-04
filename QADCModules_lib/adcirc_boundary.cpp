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
#include "adcirc_boundary.h"
#include "QADCModules_errors.h"
#include "adcirc_node.h"

//-----------------------------------------------------------------------------------------//
// Constructor for an adcirc_boundary
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for an ADCIRC boundary.
 *
 * @param[in] code   ADCIRC boundary type code
 * @param[in] size   Number of nodes in the ADCIRC boundary string
 * @param[in] parent QObject parent
 *
 * The adcirc_boundary constructor will allocate the memory for the boundary type
 * based upon its type and length as specified in the input to the constructor
 */
//-----------------------------------------------------------------------------------------//
adcirc_boundary::adcirc_boundary(QObject *parent) : QObject(parent)
{
    this->hashAlgorithm = QCryptographicHash::Sha1;
}

int adcirc_boundary::setupBoundary(int code, int size)
{
    this->code = code;
    this->numNodes = size;

    //...Resize the vectors needed for each boundary code
    if(this->code  == 0   || this->code == 1   || this->code == 2   ||
        this->code == 10  || this->code == 11  || this->code == 12  ||
        this->code == 20  || this->code == 21  || this->code == 22  ||
        this->code == 30  || this->code == 52  || this->code == 102 ||
        this->code == 112 || this->code == 122 )
    {
        this->n1.resize(this->numNodes);
    }
    else if(this->code == 3 || this->code == 13 || this->code == 23 )
    {
        this->n1.resize(this->numNodes);
        this->crest.resize(this->numNodes);
        this->supercritical.resize(this->numNodes);
    }
    else if(this->code == 4 || this->code == 24 )
    {
        this->n1.resize(this->numNodes);
        this->n2.resize(this->numNodes);
        this->crest.resize(this->numNodes);
        this->supercritical.resize(this->numNodes);
        this->subcritical.resize(this->numNodes);
    }
    else if(this->code == 5  ||
            this->code == 25 )
    {
        this->n1.resize(this->numNodes);
        this->n2.resize(this->numNodes);
        this->crest.resize(this->numNodes);
        this->supercritical.resize(this->numNodes);
        this->subcritical.resize(this->numNodes);
        this->pipeCoef.resize(this->numNodes);
        this->pipeDiam.resize(this->numNodes);
        this->pipeHeight.resize(this->numNodes);
    }
    else
    {
        this->n1.resize(this->numNodes);
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse the data from an ADCIRC boundary of any type
//-----------------------------------------------------------------------------------------//
/** \brief Public function to parse an adcirc boundary depending on its boundary code
 *
 * @param[in] data        line from the ADCIRC mesh file for this boundary type
 * @param[in] index       position in the boundary string
 * @param[in] nodes       nodal positions in the mesh
 * @param[in] nodeMapping node mapping table between the node id and node array position
 *
 * This function will read the ADCIRC boundary data (single line) from the mesh file based
 * upon the ADCIRC boundary type that is already specified.
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::fromString(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping)
{
    int ierr;

    if(this->code == -1)
        return ERROR_MESHREAD_BNDNUL;

    if(this->code  == 0       || this->code == 1   || this->code == 2   ||
            this->code == 10  || this->code == 11  || this->code == 12  ||
            this->code == 20  || this->code == 21  || this->code == 22  ||
            this->code == 30  || this->code == 52  || this->code == 102 ||
            this->code == 112 || this->code == 122 )
        ierr = this->readBoundarySingleNode(data,index,nodes,nodeMapping);
    else if(this->code == 3 || this->code == 13 || this->code == 23 )
        ierr = this->readBoundaryOneSidedWeir(data,index,nodes,nodeMapping);
    else if(this->code == 4 || this->code == 24 )
        ierr = this->readBoundaryTwoSidedWeir(data,index,nodes,nodeMapping);
    else if(this->code == 5  || this->code == 25 )
        ierr = this->readBoundaryCrossBarrierPipe(data,index,nodes,nodeMapping);
    else
        ierr = this->readBoundarySingleNode(data,index,nodes,nodeMapping);
    return ierr;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse the data from an ADCIRC mesh file for a single node boundary
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the single node data from a boundary string
 *  (single node types)
 *
 * @param[in] data        line from the ADCIRC mesh file for this boundary type
 * @param[in] index       position in the boundary string
 * @param[in] nodes       nodal positions in the mesh
 * @param[in] nodeMapping node mapping table between the node id and node array position
 *
 * Protected function to read the single node data from a land boundary string
 * (single node types)
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readBoundarySingleNode(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);

    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a one-sided weir boundary (type 3,13)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the one sided weir data from a boundary string (type 3,13)
 *
 * @param[in] data        line from the ADCIRC mesh file for this boundary type
 * @param[in] index       position in the boundary string
 * @param[in] nodes       nodal positions in the mesh
 * @param[in] nodeMapping node mapping table between the node id and node array position
 *
 * Protected function to read the one sided weir data from a land
 * boundary string (type 3,13)
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readBoundaryOneSidedWeir(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    qreal  tempDouble;

    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;
    this->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;
    this->supercritical[index] = tempDouble;

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a two-sided weir (type 4,24)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the one sided two sided weir information from a
 * boundary string (type 4,24)
 *
 * @param[in] data        line from the ADCIRC mesh file for this boundary type
 * @param[in] index       position in the boundary string
 * @param[in] nodes       nodal positions in the mesh
 * @param[in] nodeMapping node mapping table between the node id and node array position
 *
 * Protected function to read the one sided two sided weir information from a
 * land boundary string (type 4,24)
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readBoundaryTwoSidedWeir(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{

    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    qreal  tempDouble;

    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempInt     = tempString2.toInt(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->n2[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(3);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->subcritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(4);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->supercritical[index] = tempDouble;

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a two-sided weir with cross-barrier pipes (type 5,25)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the so-called "leaky weir", or weir with cross
 *  barrier pipes from a boundary string (type 5,25)
 *
 * @param[in] data        line from the ADCIRC mesh file for this boundary type
 * @param[in] index       position in the boundary string
 * @param[in] nodes       nodal positions in the mesh
 * @param[in] nodeMapping node mapping table between the node id and node array position
 *
 * Protected function to read the so-called "leaky weir", or weir with cross
 * barrier pipes from a land boundary string (type 5,25)
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readBoundaryCrossBarrierPipe(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    qreal  tempDouble;

    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempInt     = tempString2.toInt(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->n2[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(3);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->subcritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(4);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->supercritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(5);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->pipeHeight[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(6);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->pipeCoef[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(7);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_BNDERR;

    this->pipeDiam[index] = tempDouble;

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to convert the boundary to a list of strings to be written to the mesh file
//-----------------------------------------------------------------------------------------//
/** \brief Public function to construct the boundary condition into a QStringList for writing
 *
 * @param[in] isOpenBC If this is an open BC, the boundary code is not included
 *                     per the ADCIRC format
 *
 * Public function to construct the boundary condition into a QStringList for writing
 */
//-----------------------------------------------------------------------------------------//
QStringList adcirc_boundary::toStringList(bool isOpenBC)
{
    int         i;
    QString     tempString;
    QStringList outputList;

    if(isOpenBC)
        outputList.append(QString::number(this->numNodes));
    else
        outputList.append(tempString.sprintf("%11i  %11i",this->numNodes,this->code));

    for(i=0;i<this->numNodes;i++)
    {
        if(this->code  == 0   || this->code == 1   || this->code == 2   ||
            this->code == 10  || this->code == 11  || this->code == 12  ||
            this->code == 20  || this->code == 21  || this->code == 22  ||
            this->code == 30  || this->code == 52  || this->code == 102 ||
            this->code == 112 || this->code == 122 )
        {
            tempString.sprintf("%11i",this->n1[i]->id);
        }
        else if(this->code == 3 || this->code == 13 || this->code == 23 )
        {
            tempString.sprintf("%11i %6.3f %6.3f",this->n1[i]->id,
                                                this->crest[i],
                                                this->supercritical[i]);
        }
        else if(this->code == 4 || this->code == 24 )
        {
            tempString.sprintf("%11i %11i %6.3f %6.3f %6.3f",this->n1[i]->id,
                                                          this->n2[i]->id,
                                                          this->crest[i],
                                                          this->subcritical[i],
                                                          this->supercritical[i]);
        }
        else if(this->code == 5  || this->code == 25 )
        {
            tempString.sprintf("%11i %11i %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f",
                                                            this->n1[i]->id,
                                                            this->n2[i]->id,
                                                            this->crest[i],
                                                            this->subcritical[i],
                                                            this->supercritical[i],
                                                            this->pipeHeight[i],
                                                            this->pipeCoef[i],
                                                            this->pipeDiam[i]);
        }
        else
        {
            tempString.sprintf("%11i",this->n1[i]->id);
        }
        outputList.append(tempString);
    }
    return outputList;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to generate a hash for an adcirc_boundary
//-----------------------------------------------------------------------------------------//
/** \brief Public function to generate a hash for an adcirc_boundary
 *
 * Function to generate a hash for an adcirc_boundary object. The hash is accumulated
 * along the boundary string using the hashes of the adcirc_node objects that are contained
 * within it to generate a unique hash.
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::hashBoundary()
{
    QString hashSeed;
    int i;

    QCryptographicHash localHash(this->hashAlgorithm);
    localHash.reset();

    //...Hash the boundary code
    hashSeed.sprintf("%+6i",this->code);
    localHash.addData(hashSeed.toUtf8(),hashSeed.length());

    //...Accumulate the hash along the boundary string
    if( this->code  == 0   ||
        this->code  == 1   ||
        this->code  == 2   ||
        this->code  == 10  ||
        this->code  == 11  ||
        this->code  == 12  ||
        this->code  == 20  ||
        this->code  == 21  ||
        this->code  == 22  ||
        this->code  == 30  ||
        this->code  == 52  ||
        this->code  == 102 ||
        this->code  == 112 ||
        this->code  == 122    )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }
    else if( this->code == 3  ||
             this->code == 13 ||
             this->code == 23   )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->crest[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->supercritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }
    else if( this->code == 4 ||
             this->code == 24 )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed = this->n2[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->crest[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->subcritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->supercritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }
    else if( this->code == 5 ||
             this->code == 25 )
    {
        for(i=0;i<this->numNodes;i++)
        {
            hashSeed = QString();
            hashSeed = this->n1[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed = this->n2[i]->positionHash;
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->crest[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->subcritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->supercritical[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->pipeHeight[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->pipeCoef[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
            hashSeed = QString();
            hashSeed.sprintf("%+018.12e",this->pipeDiam[i]);
            localHash.addData(hashSeed.toUtf8(),hashSeed.length());
        }
    }

    this->hash = localHash.result().toHex();

    return 0;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to set the type of hash that is to be used for this class
//-----------------------------------------------------------------------------------------//
/** \brief Public function to set the hash algorithm to use
 *
 * @param[in] hashType  The QCryptographicHash::Algorithm to use
 *
 * This function sets the type of hash algorithm to use for hashes constructed for this
 * class. The hash can be MD4, MD5, SHA1, SSA256, SHA512 or any other contained with Qt
 * The default is Sha1
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::setHashAlgorithm(QCryptographicHash::Algorithm hashType)
{
    this->hashAlgorithm = hashType;
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to calculate the average longitude of a boundary
//-----------------------------------------------------------------------------------------//
/** \brief Function to calculate the average longitude of a boundary
 *
 * Function to calculate the average longitude of a boundary. This is used to ensure that
 * river boundaries remain in a consistent and known order for doing model runs
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::calculateAverageLongitude()
{
    int i;
    qreal position = 0.0;
    qreal nn = static_cast<qreal>(this->numNodes);

    for(i=0;i<this->numNodes;i++)
        position = position + this->n1[i]->position.x();

    this->averageLongitude = position/nn;

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
