#ifndef MODULESHAREDDATA_H
#define MODULESHAREDDATA_H

#include "modulenetworkparams.h"
#include "abstractfactoryserviceinterfaces.h"
#include "cro_systemobserver.h"

class ModuleSharedData
{
public:
    ModuleSharedData(ModuleNetworkParamsPtr networkParams,
                     AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                     ChannelRangeObserver::SystemObserverPtr channelRangeObserver,
                     bool demo);
    virtual ~ModuleSharedData();

    const ModuleNetworkParamsPtr m_networkParams;
    const AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    const ChannelRangeObserver::SystemObserverPtr m_channelRangeObserver;
    const bool m_demo;
private:
    static int m_instanceCount;
};

typedef std::shared_ptr<ModuleSharedData> ModuleSharedDataPtr;

#endif // MODULESHAREDDATA_H
