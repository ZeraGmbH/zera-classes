#ifndef DEMOFACTORYACTVALMANINTHEMIDDLE_H
#define DEMOFACTORYACTVALMANINTHEMIDDLE_H

#include "abstractfactoryactvalmaninthemiddle.h"

class DemoFactoryActValManInTheMiddle : public AbstractFactoryActValManInTheMiddle
{
public:
    AbstractActValManInTheMiddlePtr getActValGeneratorDft(int entityId, QStringList valueChannelList) override;
    AbstractActValManInTheMiddlePtr getActValGeneratorRms(int entityId, QStringList valueChannelList) override;
};

#endif // DEMOFACTORYACTVALMANINTHEMIDDLE_H
