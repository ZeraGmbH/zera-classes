#ifndef DEMOFACTORYACTVALMANINTHEMIDDLE_H
#define DEMOFACTORYACTVALMANINTHEMIDDLE_H

#include "abstractfactoryactvalmaninthemiddle.h"

class DemoFactoryActValManInTheMiddle : public AbstractFactoryActValManInTheMiddle
{
public:
    AbstractActValManInTheMiddlePtr getActValGeneratorRms(int entityId, QStringList valueChannelList) override;
};

#endif // DEMOFACTORYACTVALMANINTHEMIDDLE_H
