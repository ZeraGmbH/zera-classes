#ifndef DEMOFACTORYACTUALVALUEGENERATOR_H
#define DEMOFACTORYACTUALVALUEGENERATOR_H

#include "abstractfactoryactualvaluegenerator.h"

class DemoFactoryActualValueGenerator : public AbstractFactoryActualValueGenerator
{
public:
    AbstractActualValueGeneratorPtr getActValGeneratorRms(QStringList valueChannelList) override;
};

#endif // DEMOFACTORYACTUALVALUEGENERATOR_H
