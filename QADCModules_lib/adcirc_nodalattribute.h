#ifndef ADCIRC_NODALATTRIBUTE_H
#define ADCIRC_NODALATTRIBUTE_H

#include <QObject>
#include <QVector>

#include "adcirc_forward.h"

class adcirc_nodalattribute : public QObject
{
    Q_OBJECT
public:
    explicit adcirc_nodalattribute(adcirc_nodalparameter *nodalMetadata, QObject *parent = 0);

    QVector<qreal> values;

    adcirc_nodalparameter *metadata;

};

#endif // ADCIRC_NODALATTRIBUTE_H
