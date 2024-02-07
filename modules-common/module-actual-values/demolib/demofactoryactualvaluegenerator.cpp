#include "demofactoryactualvaluegenerator.h"
#include "demoactualvaluegeneratorrms.h"

AbstractActualValueGeneratorPtr DemoFactoryActualValueGenerator::getActValGeneratorRms(QStringList valueChannelList)
{
    return std::make_unique<DemoActualValueGeneratorRms>(valueChannelList);
}
