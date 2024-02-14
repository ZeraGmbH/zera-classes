#ifndef FACTORYACTUALVALUEGENERATOR_H
#define FACTORYACTUALVALUEGENERATOR_H

#include "abstractfactoryactualvaluegenerator.h"

class FactoryActualValueGenerator : public AbstractFactoryActualValueGenerator
{
public:
    AbstractActualValueGeneratorPtr getActValGeneratorRms(int entityId, QStringList valueChannelList) override;
};

#endif // FACTORYACTUALVALUEGENERATOR_H
