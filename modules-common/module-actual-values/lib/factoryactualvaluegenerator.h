#ifndef FACTORYACTUALVALUEGENERATOR_H
#define FACTORYACTUALVALUEGENERATOR_H

#include "abstractfactoryactualvaluegenerator.h"

class FactoryActualValueGenerator : public AbstractFactoryActualValueGenerator
{
public:
    AbstractActualValueGeneratorPtr getActValGeneratorRms(QStringList valueChannelList) override;
};

#endif // FACTORYACTUALVALUEGENERATOR_H