#include "adcirc_nodalattribute.h"
#include "adcirc_nodalparameter.h"

adcirc_nodalattribute::adcirc_nodalattribute(adcirc_nodalparameter *nodalMetadata, QObject *parent) : QObject(parent)
{
    this->metadata = nodalMetadata;
    this->values.resize(this->metadata->nValues);
}
