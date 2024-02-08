#include "factoryactualvaluegenerator.h"
#include "actualvaluegeneratorstartstop.h"

AbstractActualValueGeneratorPtr FactoryActualValueGenerator::getActValGeneratorRms(QStringList valueChannelList)
{
    Q_UNUSED(valueChannelList)
    return std::make_unique<ActualValueGeneratorStartStop>();
}
