#ifndef TESTFACTORYACTUALVALUEGENERATOR_H
#define TESTFACTORYACTUALVALUEGENERATOR_H

#include "abstractfactoryactualvaluegenerator.h"
#include "testactualvaluegeneratorrms.h"
#include <QHash>

class TestFactoryActualValueGenerator : public AbstractFactoryActualValueGenerator
{
public:
    AbstractActualValueGeneratorPtr getActValGeneratorRms(int entityId, QStringList valueChannelList = QStringList()) override;
    TestActualValueGeneratorRmsPtr getActValGeneratorRmsTest(int entityId);
private:
    QHash<int, TestActualValueGeneratorRmsPtr> m_ActValGenerators;
};

typedef std::shared_ptr<TestFactoryActualValueGenerator> TestFactoryActualValueGeneratorPtr;

#endif // TESTFACTORYACTUALVALUEGENERATOR_H
