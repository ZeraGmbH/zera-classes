#include "demofactoryactvalmaninthemiddle.h"
#include "demoactvalmaninthemiddledft.h"
#include "demoactvalmaninthemiddlerms.h"

AbstractActValManInTheMiddlePtr DemoFactoryActValManInTheMiddle::getActValGeneratorDft(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId);
    return std::make_shared<DemoActValManInTheMiddleDft>(valueChannelList);
}

AbstractActValManInTheMiddlePtr DemoFactoryActValManInTheMiddle::getActValGeneratorRms(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId);
    return std::make_shared<DemoActValManInTheMiddleRms>(valueChannelList);
}
