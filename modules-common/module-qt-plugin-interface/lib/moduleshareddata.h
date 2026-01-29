#ifndef MODULESHAREDDATA_H
#define MODULESHAREDDATA_H

#include "modulenetworkparams.h"
#include "abstractfactoryserviceinterfaces.h"
#include "cro_systemobserver.h"

namespace VeinStorage
{
class AbstractEventSystem;
}

class ModuleSharedData
{
public:
    ModuleSharedData(ModuleNetworkParamsPtr networkParams,
                     AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                     VeinStorage::AbstractEventSystem* storagesystem,
                     ChannelRangeObserver::SystemObserverPtr channelRangeObserver,
                     bool demo,
                     const QString &persistencyBasePath);
    virtual ~ModuleSharedData();

    const ModuleNetworkParamsPtr m_networkParams;
    const AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    const VeinStorage::AbstractEventSystem* m_storagesystem;
    const ChannelRangeObserver::SystemObserverPtr m_channelRangeObserver;
    const bool m_demo;
    const QString m_persistencyBasePath;
private:
    static int m_instanceCount;
};

typedef std::shared_ptr<ModuleSharedData> ModuleSharedDataPtr;

#endif // MODULESHAREDDATA_H
