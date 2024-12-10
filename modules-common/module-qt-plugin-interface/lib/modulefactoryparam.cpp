#include "modulefactoryparam.h"

int ModuleSharedData::m_instanceCount = 0;

ModuleSharedData::ModuleSharedData(ModuleNetworkParamsPtr networkParams,
                                   AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                                   ChannelRangeObserver::SystemObserverPtr croObserver,
                                   bool demo) :
    m_networkParams(networkParams),
    m_serviceInterfaceFactory(serviceInterfaceFactory),
    m_croObserver(croObserver),
    m_demo(demo)
{
    Q_ASSERT(m_instanceCount == 0); // crashing here: Are there module zombies on session change?
    m_instanceCount++;
}

ModuleSharedData::~ModuleSharedData()
{
    m_instanceCount--;
}

ModuleFactoryParam::ModuleFactoryParam(int entityId,
                                       int moduleNum,
                                       QByteArray configXmlData,
                                       VeinStorage::AbstractEventSystem *storagesystem,
                                       ModuleSharedDataPtr moduleSharedData) :
    m_entityId(entityId),
    m_moduleNum(moduleNum),
    m_configXmlData(configXmlData),
    m_storagesystem(storagesystem),
    m_moduleSharedData(moduleSharedData)
{
}

ModuleFactoryParam ModuleFactoryParam::getAdjustedParam(ModuleGroupNumerator *groupNumerator)
{
    return ModuleFactoryParam(m_entityId,
                              groupNumerator->requestModuleNum(m_moduleNum),
                              m_configXmlData,
                              m_storagesystem,
                              m_moduleSharedData);
}
