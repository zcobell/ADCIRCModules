#ifndef ADCIRC_OUTPUT_RECORD_H
#define ADCIRC_OUTPUT_RECORD_H

#include <QObject>
#include <QVector>

#include "QADCModules_global.h"
#include "QADCModules_errors.h"

class QADCMODULESSHARED_EXPORT adcirc_output_record : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_output_record(int numNodes, QObject *parent = 0);

    //...PUBLIC VARIABLES...//

    ///Number of nodes in the output records
    int numNodes;

    ///Model time (seconds) when this record was written
    double modelTime;

    ///Vector containig the model data for this output record
    QVector<double> scalar;

    ///Vector containing the model data for this output record (u-component of the vector)
    QVector<double> vector_u;

    ///Vector containing the model data for this output record (v-component of the vector)
    QVector<double> vector_v;

    //...PUBLIC FUNCTIONS...//
    int toMagnitude();

};

#endif // ADCIRC_OUTPUT_RECORD_H
