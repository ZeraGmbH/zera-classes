#ifndef SEMFUNCTIONS_H
#define SEMFUNCTIONS_H

#include "abstractsemspmfunctions.h"

class SemFunctions : public AbstractSemSpmFunctions
{
public:
    const QHash<QString, double> &getUnitFactorHash() const override;
};

#endif // SEMFUNCTIONS_H
