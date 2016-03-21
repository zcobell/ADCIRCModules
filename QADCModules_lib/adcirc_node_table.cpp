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
#include "adcirc_node_table.h"


//-----------------------------------------------------------------------------------------//
// Constructor for an adcirc_node_table
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for an adcirc_node_table
 *
 * @param[in] node   ADCIRC node that this table describes
 * @param[in] parent QObject parent
 *
 * Constructor for an adcirc_node_table
 *
 */
//-----------------------------------------------------------------------------------------//
adcirc_node_table::adcirc_node_table(adcirc_node *node, QObject *parent) : QObject(parent)
{
    this->node = node;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Adds an adcirc_element to the table
//-----------------------------------------------------------------------------------------//
/** \brief Adds an adcirc_element to the table
 *
 * @param[in] addElement adcirc_element to add to the table
 *
 * Adds an adcirc_element object to the table
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_node_table::add(adcirc_element *addElement)
{
    if(!this->elementsAroundNode.contains(addElement))
        this->elementsAroundNode.append(addElement);
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Adds an adcirc_element to the table
//-----------------------------------------------------------------------------------------//
/** \brief Removes an adcirc_element from the table
 *
 * @param[in] removeElement adcirc_element to remove from the table
 *
 * Removes an adcirc_element object from the table
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_node_table::remove(adcirc_element *removeElement)
{
    if(this->elementsAroundNode.contains(removeElement))
        this->elementsAroundNode.removeAll(removeElement);
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Removes all elements from an adcirc_node_table
//-----------------------------------------------------------------------------------------//
/** \brief Removes all elements from an adcirc_node_table
 *
 * Removes all adcirc_element objects from an adcirc_node_table
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_node_table::clear()
{
    this->elementsAroundNode.clear();
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
