#ifndef SPMFUNCTIONS_H
#define SPMFUNCTIONS_H

#include "abstractsemspmfunctions.h"

class SpmFunctions : public AbstractSemSpmFunctions
{
public:
    const QHash<QString, double> &getUnitFactorHash() const override;
};

#endif // SPMFUNCTIONS_H
