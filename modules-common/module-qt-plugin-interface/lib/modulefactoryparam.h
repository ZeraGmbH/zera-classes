#ifndef MEASUREMENTMODULEFACTORYPARAM_H
#define MEASUREMENTMODULEFACTORYPARAM_H

#include "modulenetworkparams.h"
#include "modulegroupnumerator.h"
#include "abstractfactoryserviceinterfaces.h"
#include "cro_systemobserver.h"
#include <abstracttcpnetworkfactory.h>

namespace VeinStorage
{
class AbstractEventSystem;
}

class ModuleSharedData
{
public:
    ModuleSharedData(ModuleNetworkParamsPtr networkParams,
                     AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                     ChannelRangeObserver::SystemObserverPtr croObserver,
                     bool demo);
    virtual ~ModuleSharedData();

    const ModuleNetworkParamsPtr m_networkParams;
    const AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    const ChannelRangeObserver::SystemObserverPtr m_croObserver;
    const bool m_demo;
private:
    static int m_instanceCount;
};

typedef std::shared_ptr<ModuleSharedData> ModuleSharedDataPtr;

struct ModuleFactoryParam
{
    ModuleFactoryParam(int entityId,
                       int moduleNum,
                       QByteArray configXmlData,
                       VeinStorage::AbstractEventSystem* storagesystem,
                       ModuleSharedDataPtr moduleSharedData);
    ModuleFactoryParam getAdjustedParam(ModuleGroupNumerator* groupNumerator);
    const int m_entityId;
    const int m_moduleNum;
    const QByteArray m_configXmlData;
    VeinStorage::AbstractEventSystem* m_storagesystem;
    const ModuleSharedDataPtr m_moduleSharedData;
};

#endif // MEASUREMENTMODULEFACTORYPARAM_H
