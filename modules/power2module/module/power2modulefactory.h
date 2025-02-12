#ifndef POWER2MODULEFACTORY_H
#define POWER2MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace POWER2MODULE
{

class Power2ModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "power2module.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) override;
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // POWERMODULEFACTORY_H
