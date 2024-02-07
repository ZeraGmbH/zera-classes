#ifndef TESTFACTORYACTUALVALUEGENERATOR_H
#define TESTFACTORYACTUALVALUEGENERATOR_H

#include "abstractfactoryactualvaluegenerator.h"

class TestFactoryActualValueGenerator : public AbstractFactoryActualValueGenerator
{
public:
    AbstractActualValueGeneratorPtr getActValGeneratorRms(QStringList valueChannelList) override;
};

#endif // TESTFACTORYACTUALVALUEGENERATOR_H
