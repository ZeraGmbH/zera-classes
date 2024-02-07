#ifndef TESTACTUALVALUEGENERATORRMS_H
#define TESTACTUALVALUEGENERATORRMS_H

#include "abstractactualvaluegenerator.h"

class TestActualValueGeneratorRms : public AbstractActualValueGenerator
{
    Q_OBJECT
public:
    void start() override;
    void stop() override;
};

#endif // TESTACTUALVALUEGENERATORRMS_H
