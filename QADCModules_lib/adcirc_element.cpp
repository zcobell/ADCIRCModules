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
#include "adcirc_element.h"
#include "QADCModules_errors.h"

bool operator<(const adcirc_element e1,const adcirc_element e2)
{
    return e1.id<e2.id;
}

bool operator>(const adcirc_element e1,const adcirc_element e2)
{
    return e1.id>e2.id;
}

bool operator==(const adcirc_element e1,const adcirc_element e2)
{
    int i;

    if(e1.numConnections!=e2.numConnections)
        return false;

    QList<int> l1,l2;
    for(i=0;i<e1.numConnections;i++)
        l1.append(e1.connections[i]->id);
    for(i=0;i<e2.numConnections;i++)
        l2.append(e2.connections[i]->id);

    for(i=0;i<l1.length();i++)
        if(!l2.contains(l1.at(i)))
            return false;

    return true;
}

//-----------------------------------------------------------------------------------------//
// Constructor function. Initializes the adcirc_element class and sets defaults
//-----------------------------------------------------------------------------------------//
/** \brief Constructor function for adcirc_element class
 *
 * \author Zach Cobell
 *
 * @param[in] parent Parent QObject used for memory management
 *
 * Constructor function for adcirc_element class. Assumes triangular elements.
 *
 */
//-----------------------------------------------------------------------------------------//
adcirc_element::adcirc_element(QObject *parent) : QObject(parent)
{
    this->numConnections = 3;
    this->connections.resize(3);
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Parses a line from the ADCIRC mesh that contains an element into
//   its connectivity information. Note that if the mesh needs re-numbering,
//   you will need to set ignoreMeshNumbering to true;
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to parse the string from an ADCIRC mesh file containing
 *  elemental connectivity information
 *
 * \author Zach Cobell
 *
 * @param[in]     line     The QString with the information read from the file to be parsed into an ADCIRC element
 * @param[in]     index    An integer for the current element position in the ADCIRC file
 * @param[in,out] *element The pointer to the adcirc_element that should be created with this information
 *
 * Protected function to parse the string from an ADCIRC mesh file containing
 *  elemental connectivity information
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_element::fromString(QString line, QVector<adcirc_node *> &nodes, QMap<int, int> &nodeMapping)
{
    QStringList tempList;
    QString     tempString;
    int         tempInt;
    bool        err;

    tempList   = line.simplified().split(" ");
    tempString = tempList.value(0);
    tempInt    = tempString.toInt(&err);
    if(!err)
        return ERROR_MESHREAD_ELEMER;

    this->id = tempInt;

    tempString = tempList.value(2);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return ERROR_MESHREAD_ELEMER;
    this->connections[0] = nodes[nodeMapping[tempInt]];

    tempString = tempList.value(3);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return ERROR_MESHREAD_ELEMER;
    this->connections[1] = nodes[nodeMapping[tempInt]];

    tempString = tempList.value(4);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return ERROR_MESHREAD_ELEMER;
    this->connections[2] = nodes[nodeMapping[tempInt]];

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function that writes an ADCIRC element to a string
//-----------------------------------------------------------------------------------------//
/** \brief Protected function that writes an ADCIRC element in the format expected by an
 *  ADCIRC mesh file
 *
 * \author Zach Cobell
 *
 * Protected function that writes an ADCIRC element in the format expected by an
 * ADCIRC mesh file
 */
//-----------------------------------------------------------------------------------------//
QString adcirc_element::toString()
{
    QString output;
    output.sprintf("%11i %3i %11i %11i %11i",this->id,this->numConnections,
                                        this->connections[0]->id,
                                        this->connections[1]->id,
                                        this->connections[2]->id);
    return output;
}
