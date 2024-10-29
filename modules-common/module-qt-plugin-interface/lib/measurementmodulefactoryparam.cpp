#include "measurementmodulefactoryparam.h"

MeasurementModuleFactoryParam::MeasurementModuleFactoryParam(int entityId,
                                                             int moduleNum,
                                                             QByteArray configXmlData,
                                                             VeinEvent::StorageSystem *storagesystem,
                                                             AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                                                             VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                                             bool demo) :
    m_entityId(entityId),
    m_moduleNum(moduleNum),
    m_configXmlData(configXmlData),
    m_storagesystem(storagesystem),
    m_serviceInterfaceFactory(serviceInterfaceFactory),
    m_tcpNetworkFactory(tcpNetworkFactory),
    m_demo(demo)
{
}

MeasurementModuleFactoryParam MeasurementModuleFactoryParam::getAdjustedParam(ModuleGroupNumerator *groupNumerator)
{
    return MeasurementModuleFactoryParam(m_entityId,
                                         groupNumerator->requestModuleNum(m_moduleNum),
                                         m_configXmlData,
                                         m_storagesystem,
                                         m_serviceInterfaceFactory,
                                         m_tcpNetworkFactory,
                                         m_demo);
}
