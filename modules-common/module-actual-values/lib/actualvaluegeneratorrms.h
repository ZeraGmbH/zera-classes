#ifndef ACTUALVALUEGENERATORRMS_H
#define ACTUALVALUEGENERATORRMS_H

#include "abstractactualvaluegenerator.h"

class ActualValueGeneratorRms : public AbstractActualValueGenerator
{
    Q_OBJECT
public:
    void start() override;
    void stop() override;
};

#endif // ACTUALVALUEGENERATORRMS_H
