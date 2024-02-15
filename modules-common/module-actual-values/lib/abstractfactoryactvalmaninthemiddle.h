#ifndef ABSTRACTFACTORYACTVALMANINTHEMIDDLE_H
#define ABSTRACTFACTORYACTVALMANINTHEMIDDLE_H

#include "abstractactvalmaninthemiddle.h"

class AbstractFactoryActValManInTheMiddle
{
public:
    virtual AbstractActValManInTheMiddlePtr getActValGeneratorDft(int entityId, QStringList valueChannelList) = 0;
    virtual AbstractActValManInTheMiddlePtr getActValGeneratorRms(int entityId, QStringList valueChannelList) = 0;
};

typedef std::shared_ptr<AbstractFactoryActValManInTheMiddle> AbstractFactoryActValManInTheMiddlePtr;

#endif // ABSTRACTFACTORYACTVALMANINTHEMIDDLE_H
