#include "demofactoryactualvaluegenerator.h"
#include "demoactualvaluegeneratorrms.h"

AbstractActualValueGeneratorPtr DemoFactoryActualValueGenerator::getActValGeneratorRms(int entityId, QStringList valueChannelList)
{
    return std::make_shared<DemoActualValueGeneratorRms>(valueChannelList);
}
