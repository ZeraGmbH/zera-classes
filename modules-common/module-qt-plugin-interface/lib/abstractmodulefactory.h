#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include "virtualmodule.h"
#include "modulegroupnumerator.h"
#include <QtPlugin>
#include <QList>

namespace VeinEvent
{
    class StorageSystem;
}

class MeasurementModuleFactory
{
public:
    void setModuleNumerator(ModuleGroupNumeratorPtr moduleGroupNumerator) { m_moduleGroupNumerator = std::move(moduleGroupNumerator); }
    virtual ~MeasurementModuleFactory() = default;
    virtual ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum) = 0;
    virtual void destroyModule(ZeraModules::VirtualModule *module) = 0;
    virtual QString getFactoryName() const = 0;
protected:
    ModuleGroupNumeratorPtr m_moduleGroupNumerator;
};

#define MeasurementModuleFactory_iid "org.qt-project.Qt.Examples.Test/1.0"
Q_DECLARE_INTERFACE(MeasurementModuleFactory, MeasurementModuleFactory_iid)

#endif // MODULEINTERFACE_H
