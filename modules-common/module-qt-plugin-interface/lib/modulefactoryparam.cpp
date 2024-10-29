#include "modulefactoryparam.h"

ModuleFactoryParam::ModuleFactoryParam(int entityId,
                                       int moduleNum,
                                       QByteArray configXmlData,
                                       VeinEvent::StorageSystem *storagesystem,
                                       AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                                       ModuleFactoryParamNetworkPtr networkParams,
                                       bool demo) :
    m_entityId(entityId),
    m_moduleNum(moduleNum),
    m_configXmlData(configXmlData),
    m_storagesystem(storagesystem),
    m_serviceInterfaceFactory(serviceInterfaceFactory),
    m_networkParams(networkParams),
    m_demo(demo)
{
}

ModuleFactoryParam ModuleFactoryParam::getAdjustedParam(ModuleGroupNumerator *groupNumerator)
{
    return ModuleFactoryParam(m_entityId,
                              groupNumerator->requestModuleNum(m_moduleNum),
                              m_configXmlData,
                              m_storagesystem,
                              m_serviceInterfaceFactory,
                              m_networkParams,
                              m_demo);
}
