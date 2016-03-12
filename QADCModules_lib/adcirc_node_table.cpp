#include "adcirc_node_table.h"

adcirc_node_table::adcirc_node_table(adcirc_node *node, QObject *parent) : QObject(parent)
{
    this->node = node;
}


int adcirc_node_table::add(adcirc_element *addElement)
{
    if(!this->elementsAroundNode.contains(addElement))
        this->elementsAroundNode.append(addElement);
    return ERROR_NOERROR;
}


int adcirc_node_table::remove(adcirc_element *removeElement)
{
    if(this->elementsAroundNode.contains(removeElement))
        this->elementsAroundNode.removeAll(removeElement);
    return ERROR_NOERROR;
}


int adcirc_node_table::clear()
{
    this->elementsAroundNode.clear();
    return ERROR_NOERROR;
}
