#include "factoryactualvaluegenerator.h"
#include "actualvaluegeneratorstartstop.h"

AbstractActualValueGeneratorPtr FactoryActualValueGenerator::getActValGeneratorRms(int entityId, QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_shared<ActualValueGeneratorStartStop>();
}
