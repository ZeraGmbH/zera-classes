#ifndef RMSMODULEFACTORY_H
#define RMSMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace RMSMODULE
{

class RmsModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "rmsmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) override;
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // RMSMODULEFACTORY_H
