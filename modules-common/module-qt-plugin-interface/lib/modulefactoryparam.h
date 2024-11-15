#ifndef MEASUREMENTMODULEFACTORYPARAM_H
#define MEASUREMENTMODULEFACTORYPARAM_H

#include "modulenetworkparams.h"
#include "modulegroupnumerator.h"
#include "abstractfactoryserviceinterfaces.h"
#include <abstracttcpnetworkfactory.h>

namespace VeinStorage
{
class AbstractEventSystem;
}

struct ModuleFactoryParam
{
    ModuleFactoryParam(int entityId,
                       int moduleNum,
                       QByteArray configXmlData,
                       VeinStorage::AbstractEventSystem* storagesystem,
                       AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                       ModuleNetworkParamsPtr networkParams,
                       bool demo);
    ModuleFactoryParam getAdjustedParam(ModuleGroupNumerator* groupNumerator);
    const int m_entityId;
    const int m_moduleNum;
    const QByteArray m_configXmlData;
    VeinStorage::AbstractEventSystem* m_storagesystem;
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    const ModuleNetworkParamsPtr m_networkParams;
    const bool m_demo;
};

#endif // MEASUREMENTMODULEFACTORYPARAM_H
