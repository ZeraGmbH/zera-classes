#include "factoryactvalmaninthemiddle.h"
#include "actvalmaninthemiddlestartstop.h"

AbstractActValManInTheMiddlePtr FactoryActValManInTheMiddle::getActValGeneratorDft(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(entityId)
    Q_UNUSED(valueChannelList)
    return std::make_shared<ActValManInTheMiddleStartStop>();
}
