#include "testfactoryactualvaluegenerator.h"
#include "testactualvaluegeneratorrms.h"

AbstractActualValueGeneratorPtr TestFactoryActualValueGenerator::getActValGeneratorRms(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_unique<TestActualValueGeneratorRms>();
}
