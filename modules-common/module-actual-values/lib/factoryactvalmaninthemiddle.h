#ifndef FACTORYACTVALMANINTHEMIDDLE_H
#define FACTORYACTVALMANINTHEMIDDLE_H

#include "abstractfactoryactvalmaninthemiddle.h"

class FactoryActValManInTheMiddle : public AbstractFactoryActValManInTheMiddle
{
public:
    AbstractActValManInTheMiddlePtr getActValGeneratorRms(int entityId, QStringList valueChannelList) override;
};

#endif // FACTORYACTVALMANINTHEMIDDLE_H
