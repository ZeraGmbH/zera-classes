#include "demofactoryactvalmaninthemiddle.h"
#include "demoactvalmaninthemiddlerms.h"

AbstractActValManInTheMiddlePtr DemoFactoryActValManInTheMiddle::getActValGeneratorRms(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId);
    return std::make_shared<DemoActValManInTheMiddleRms>(valueChannelList);
}
