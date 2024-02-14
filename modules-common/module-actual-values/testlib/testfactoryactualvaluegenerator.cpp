#include "testfactoryactualvaluegenerator.h"
#include "testactualvaluegeneratorrms.h"

AbstractActualValueGeneratorPtr TestFactoryActualValueGenerator::getActValGeneratorRms(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    if(!m_ActValGenerators.contains(entityId))
        m_ActValGenerators[entityId] = std::make_shared<TestActualValueGeneratorRms>();
    return m_ActValGenerators[entityId];
}
