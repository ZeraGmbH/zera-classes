#ifndef TESTFACTORYACTUALVALUEGENERATOR_H
#define TESTFACTORYACTUALVALUEGENERATOR_H

#include "abstractfactoryactualvaluegenerator.h"
#include <QHash>

class TestFactoryActualValueGenerator : public AbstractFactoryActualValueGenerator
{
public:
    AbstractActualValueGeneratorPtr getActValGeneratorRms(int entityId, QStringList valueChannelList) override;
private:
    QHash<int, AbstractActualValueGeneratorPtr> m_ActValGenerators;
};

#endif // TESTFACTORYACTUALVALUEGENERATOR_H
