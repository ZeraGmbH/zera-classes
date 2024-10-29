#ifndef MEASUREMENTMODULEFACTORYPARAM_H
#define MEASUREMENTMODULEFACTORYPARAM_H

#include "modulefactoryparamnetwork.h"
#include "modulegroupnumerator.h"
#include "abstractfactoryserviceinterfaces.h"
#include <abstracttcpnetworkfactory.h>

namespace VeinEvent
{
class StorageSystem;
}

struct ModuleFactoryParam
{
    ModuleFactoryParam(int entityId,
                       int moduleNum,
                       QByteArray configXmlData,
                       VeinEvent::StorageSystem* storagesystem,
                       AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                       ModuleFactoryParamNetworkPtr networkParams,
                       bool demo);
    ModuleFactoryParam getAdjustedParam(ModuleGroupNumerator* groupNumerator);
    const int m_entityId;
    const int m_moduleNum;
    const QByteArray m_configXmlData;
    VeinEvent::StorageSystem* m_storagesystem;
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    const ModuleFactoryParamNetworkPtr m_networkParams;
    const bool m_demo;
};

#endif // MEASUREMENTMODULEFACTORYPARAM_H
