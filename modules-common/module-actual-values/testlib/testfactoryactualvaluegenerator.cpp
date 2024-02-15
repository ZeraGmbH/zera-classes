#include "testfactoryactualvaluegenerator.h"
#include "testactualvaluegeneratorrms.h"

AbstractActualValueGeneratorPtr TestFactoryActualValueGenerator::getActValGeneratorRms(int entityId, QStringList valueChannelList)
{
    if(!m_ActValGenerators.contains(entityId)) {
        if(valueChannelList.isEmpty())
            qFatal("Do not create value generator from tests!");
        m_ActValGenerators[entityId] = std::make_shared<TestActualValueGeneratorRms>(valueChannelList);
    }
    return m_ActValGenerators[entityId];
}

TestActualValueGeneratorRmsPtr TestFactoryActualValueGenerator::getActValGeneratorRmsTest(int entityId)
{
    if(m_ActValGenerators.isEmpty())
        qFatal("Do not create value generator from tests!");
    return m_ActValGenerators[entityId];
}
