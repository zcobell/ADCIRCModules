#include "adcirc_output_record.h"
#include <qmath.h>

adcirc_output_record::adcirc_output_record(int numNodes, QObject *parent) : QObject(parent)
{
    this->numNodes = numNodes;
}


int adcirc_output_record::toMagnitude()
{
    if(this->numNodes != this->vector_u.size() || this->numNodes != this->vector_v.size())
        return ERROR_ADCIRCOUTPUT_SIZEMISMATCH;

    this->scalar.clear();

    this->scalar.resize(this->numNodes);

    for(int i=0;i<this->numNodes;i++)
        this->scalar[i] = qPow(qPow(this->vector_u[i],2) + qPow(this->vector_v[i],2),0.50);

    return ERROR_NOERROR;
}
