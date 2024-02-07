#include "factoryactualvaluegenerator.h"
#include "actualvaluegeneratorrms.h"

AbstractActualValueGeneratorPtr FactoryActualValueGenerator::getActValGeneratorRms(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_unique<ActualValueGeneratorRms>();
}
