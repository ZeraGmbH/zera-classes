#include "moduleshareddata.h"

int ModuleSharedData::m_instanceCount = 0;

ModuleSharedData::ModuleSharedData(ModuleNetworkParamsPtr networkParams,
                                   AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory, VeinStorage::AbstractEventSystem *storagesystem,
                                   ChannelRangeObserver::SystemObserverPtr channelRangeObserver,
                                   bool demo, QString persistencyBasePath) :
    m_networkParams(networkParams),
    m_serviceInterfaceFactory(serviceInterfaceFactory),
    m_storagesystem(storagesystem),
    m_channelRangeObserver(channelRangeObserver),
    m_demo(demo),
    m_persistencyBasePath(persistencyBasePath)
{
    Q_ASSERT(m_instanceCount == 0); // crashing here: Are there module zombies on session change?
    m_instanceCount++;
}

ModuleSharedData::~ModuleSharedData()
{
    m_instanceCount--;
}
