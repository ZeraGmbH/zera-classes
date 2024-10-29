#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include "virtualmodule.h"
#include "modulegroupnumerator.h"
#include "measurementmodulefactoryparam.h"
#include <QtPlugin>

namespace VeinEvent
{
    class StorageSystem;
}

class AbstractModuleFactory // each module needs to implement a conrete
{
public:
    void setModuleNumerator(ModuleGroupNumeratorPtr moduleGroupNumerator) { m_moduleGroupNumerator = std::move(moduleGroupNumerator); }
    virtual ~AbstractModuleFactory() = default;
    virtual ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) = 0;
    virtual void destroyModule(ZeraModules::VirtualModule *module) = 0;
    virtual QString getFactoryName() const = 0;
protected:
    ModuleGroupNumeratorPtr m_moduleGroupNumerator;
};

#define MeasurementModuleFactory_iid "org.qt-project.Qt.Examples.Test/1.0"
Q_DECLARE_INTERFACE(AbstractModuleFactory, MeasurementModuleFactory_iid)

#endif // MODULEINTERFACE_H
