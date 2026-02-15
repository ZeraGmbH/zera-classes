#ifndef PERIODAVERAGEMODULEFACTORY_H
#define PERIODAVERAGEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace PERIODAVERAGEMODULE
{

class PeriodAverageModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "periodaveragemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) override;
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // PERIODAVERAGEMODULEFACTORY_H
