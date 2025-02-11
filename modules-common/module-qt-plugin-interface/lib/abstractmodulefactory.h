#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include "virtualmodule.h"
#include "modulegroupnumerator.h"
#include "modulefactoryparam.h"
#include <tasklambdarunner.h>
#include <QtPlugin>

class AbstractModuleFactory : public QObject // each module has to implement a conrete
{
    Q_OBJECT
public:
    void setModuleNumerator(ModuleGroupNumeratorPtr moduleGroupNumerator) { m_moduleGroupNumerator = std::move(moduleGroupNumerator); }
    virtual ~AbstractModuleFactory() = default;
    virtual TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) {
        return TaskLambdaRunner::create([]() { return true; });
    }
    virtual ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) = 0;
    virtual void destroyModule(ZeraModules::VirtualModule *module) = 0;
    virtual QString getFactoryName() const = 0;
protected:
    ModuleGroupNumeratorPtr m_moduleGroupNumerator;
};

#define MeasurementModuleFactory_iid "org.qt-project.Qt.Examples.Test/1.0"
Q_DECLARE_INTERFACE(AbstractModuleFactory, MeasurementModuleFactory_iid)

#endif // MODULEINTERFACE_H
