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
#include <QDebug>

//-----------------------------------------------------------------------------------------//
// Constructor for an adcirc_boundary
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for an ADCIRC boundary.
 *
 * \author Zach Cobell
 *
 * @param code   [in] ADCIRC boundary type code
 * @param size   [in] Number of nodes in the ADCIRC boundary string
 * @param parent [in] QObject parent
 *
 * The adcirc_boundary constructor will allocate the memory for the boundary type
 * based upon its type and length as specified in the input to the constructor
 */
//-----------------------------------------------------------------------------------------//
adcirc_boundary::adcirc_boundary(int code, int size, QObject *parent) : QObject(parent)
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


}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse the data from an ADCIRC boundary of any type
//-----------------------------------------------------------------------------------------//
/** \brief Public function to parse an adcirc boundary depending on its boundary code
 *
 * \author Zach Cobell
 *
 * @param data        [in] line from the ADCIRC mesh file for this boundary type
 * @param index       [in] position in the boundary string
 * @param nodes       [in] nodal positions in the mesh
 * @param nodeMapping [in] node mapping table between the node id and node array position
 *
 * This function will read the ADCIRC boundary data (single line) from the mesh file based
 * upon the ADCIRC boundary type that is already specified.
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readLandBoundaryString(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping)
{
    int ierr;

    if(this->code == -1)
        return ADCMESH_MESHREAD_BNDNUL;

    if(this->code  == 0       || this->code == 1   || this->code == 2   ||
            this->code == 10  || this->code == 11  || this->code == 12  ||
            this->code == 20  || this->code == 21  || this->code == 22  ||
            this->code == 30  || this->code == 52  || this->code == 102 ||
            this->code == 112 || this->code == 122 )
        ierr = this->readLandBoundarySingleNode(data,index,nodes,nodeMapping);
    else if(this->code == 3 || this->code == 13 || this->code == 23 )
        ierr = this->readLandBoundaryOneSidedWeir(data,index,nodes,nodeMapping);
    else if(this->code == 4 || this->code == 24 )
        ierr = this->readLandBoundaryTwoSidedWeir(data,index,nodes,nodeMapping);
    else if(this->code == 5  || this->code == 25 )
        ierr = this->readLandBoundaryCrossBarrierPipe(data,index,nodes,nodeMapping);
    else
        ierr = this->readLandBoundarySingleNode(data,index,nodes,nodeMapping);
    return ierr;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse the data from an ADCIRC mesh file for a single node boundary
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the single node data from a land boundary string (single node types)
 *
 * \author Zach Cobell
 *
 * @param data        [in] line from the ADCIRC mesh file for this boundary type
 * @param index       [in] position in the boundary string
 * @param nodes       [in] nodal positions in the mesh
 * @param nodeMapping [in] node mapping table between the node id and node array position
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readLandBoundarySingleNode(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);

    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a one-sided weir boundary (type 3,13)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the one sided weir data from a land boundary string (type 3,13)
 *
 * \author Zach Cobell
 *
 * @param data        [in] line from the ADCIRC mesh file for this boundary type
 * @param index       [in] position in the boundary string
 * @param nodes       [in] nodal positions in the mesh
 * @param nodeMapping [in] node mapping table between the node id and node array position
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readLandBoundaryOneSidedWeir(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    double  tempDouble;

    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;
    this->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;
    this->supercritical[index] = tempDouble;

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a two-sided weir (type 4,24)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the one sided two sided weir information from a
 * land boundary string (type 4,24)
 *
 * \author Zach Cobell
 *
 * @param data        [in] line from the ADCIRC mesh file for this boundary type
 * @param index       [in] position in the boundary string
 * @param nodes       [in] nodal positions in the mesh
 * @param nodeMapping [in] node mapping table between the node id and node array position
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readLandBoundaryTwoSidedWeir(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{

    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    double  tempDouble;

    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempInt     = tempString2.toInt(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->n2[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(3);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->subcritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(4);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->supercritical[index] = tempDouble;

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a two-sided weir with cross-barrier pipes (type 5,25)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the so-called "leaky weir", or weir with cross
 *  barrier pipes from a land boundary string (type 5,25)
 *
 * \author Zach Cobell
 *
 * @param data        [in] line from the ADCIRC mesh file for this boundary type
 * @param index       [in] position in the boundary string
 * @param nodes       [in] nodal positions in the mesh
 * @param nodeMapping [in] node mapping table between the node id and node array position
 */
//-----------------------------------------------------------------------------------------//
int adcirc_boundary::readLandBoundaryCrossBarrierPipe(QString data, int index, QVector<adcirc_node *> &nodes, QMap<int,int> &nodeMapping)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    double  tempDouble;

    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->n1[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempInt     = tempString2.toInt(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->n2[index] = nodes[nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(3);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->subcritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(4);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->supercritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(5);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->pipeHeight[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(6);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->pipeCoef[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(7);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_BNDERR;

    this->pipeDiam[index] = tempDouble;

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//
