#include "testfactoryactvalmaninthemiddle.h"
#include "testactvalmaninthemiddle.h"

AbstractActValManInTheMiddlePtr TestFactoryActValManInTheMiddle::getActValGeneratorRms(int entityId, QStringList valueChannelList)
{
    if(!m_ActValGenerators.contains(entityId)) {
        if(valueChannelList.isEmpty())
            qFatal("Do not create value men in the middle from tests!");
        m_ActValGenerators[entityId] = std::make_shared<TestActValManInTheMiddle>(valueChannelList);
    }
    return m_ActValGenerators[entityId];
}

TestActValManInTheMiddlePtr TestFactoryActValManInTheMiddle::getActValGeneratorRmsTest(int entityId)
{
    if(m_ActValGenerators.isEmpty())
        qFatal("Do not create value men in the middle from tests!");
    return m_ActValGenerators[entityId];
}
