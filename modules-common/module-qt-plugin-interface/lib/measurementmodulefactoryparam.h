#ifndef MEASUREMENTMODULEFACTORYPARAM_H
#define MEASUREMENTMODULEFACTORYPARAM_H

#include "modulegroupnumerator.h"
#include "abstractfactoryserviceinterfaces.h"
#include <abstracttcpnetworkfactory.h>

namespace VeinEvent
{
class StorageSystem;
}

struct MeasurementModuleFactoryParam
{
    MeasurementModuleFactoryParam(int entityId,
                                  int moduleNum,
                                  QByteArray configXmlData,
                                  VeinEvent::StorageSystem* storagesystem,
                                  AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                                  VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                                  bool demo);
    MeasurementModuleFactoryParam getAdjustedParam(ModuleGroupNumerator* groupNumerator);
    const int m_entityId;
    const int m_moduleNum;
    const QByteArray m_configXmlData;
    VeinEvent::StorageSystem* m_storagesystem;
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    const bool m_demo;
};

#endif // MEASUREMENTMODULEFACTORYPARAM_H
