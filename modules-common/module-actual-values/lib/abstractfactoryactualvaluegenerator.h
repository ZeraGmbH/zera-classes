#ifndef ABSTRACTFACTORYACTUALVALUEGENERATOR_H
#define ABSTRACTFACTORYACTUALVALUEGENERATOR_H

#include "abstractactualvaluegenerator.h"

class AbstractFactoryActualValueGenerator
{
public:
    virtual AbstractActualValueGeneratorPtr getActValGeneratorRms(QStringList valueChannelList) = 0;
};

typedef std::shared_ptr<AbstractFactoryActualValueGenerator> AbstractFactoryActualValueGeneratorPtr;

#endif // ABSTRACTFACTORYACTUALVALUEGENERATOR_H
