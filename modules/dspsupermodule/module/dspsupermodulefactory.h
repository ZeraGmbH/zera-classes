#ifndef DSPSUPERMODULEFACTORY_H
#define DSPSUPERMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace DSPSUPERMODULE
{

class DspSuperModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "dspsupermodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) override;
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // DSPSUPERMODULEFACTORY_H
