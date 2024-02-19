#include "demofactoryactvalmaninthemiddle.h"
#include "demoactvalmaninthemiddledft.h"

AbstractActValManInTheMiddlePtr DemoFactoryActValManInTheMiddle::getActValGeneratorDft(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId);
    return std::make_shared<DemoActValManInTheMiddleDft>(valueChannelList);
}

