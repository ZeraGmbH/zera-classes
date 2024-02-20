#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include "virtualmodule.h"
#include "modulegroupnumerator.h"
#include "abstractfactoryserviceinterfaces.h"
#include <QtPlugin>

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
                                  bool demo);
    MeasurementModuleFactoryParam getAdjustedParam(ModuleGroupNumerator* groupNumerator);
    const int m_entityId;
    const int m_moduleNum;
    const QByteArray m_configXmlData;
    VeinEvent::StorageSystem* m_storagesystem;
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
    const bool m_demo;
};

inline MeasurementModuleFactoryParam::MeasurementModuleFactoryParam(int entityId,
                                                                    int moduleNum,
                                                                    QByteArray configXmlData,
                                                                    VeinEvent::StorageSystem *storagesystem,
                                                                    AbstractFactoryServiceInterfacesPtr serviceInterfaceFactory,
                                                                    bool demo) :
    m_entityId(entityId),
    m_moduleNum(moduleNum),
    m_configXmlData(configXmlData),
    m_storagesystem(storagesystem),
    m_serviceInterfaceFactory(serviceInterfaceFactory),
    m_demo(demo)
{
}

inline MeasurementModuleFactoryParam MeasurementModuleFactoryParam::getAdjustedParam(ModuleGroupNumerator *groupNumerator)
{
    return MeasurementModuleFactoryParam(m_entityId,
                                         groupNumerator->requestModuleNum(m_moduleNum),
                                         m_configXmlData,
                                         m_storagesystem,
                                         m_serviceInterfaceFactory,
                                         m_demo);
}

class MeasurementModuleFactory
{
public:
    void setModuleNumerator(ModuleGroupNumeratorPtr moduleGroupNumerator) { m_moduleGroupNumerator = std::move(moduleGroupNumerator); }
    virtual ~MeasurementModuleFactory() = default;
    virtual ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) = 0;
    virtual void destroyModule(ZeraModules::VirtualModule *module) = 0;
    virtual QString getFactoryName() const = 0;
protected:
    ModuleGroupNumeratorPtr m_moduleGroupNumerator;
};

#define MeasurementModuleFactory_iid "org.qt-project.Qt.Examples.Test/1.0"
Q_DECLARE_INTERFACE(MeasurementModuleFactory, MeasurementModuleFactory_iid)

#endif // MODULEINTERFACE_H
