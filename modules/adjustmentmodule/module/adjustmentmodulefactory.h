#ifndef ADJUSTMENTMODULEFACTORY_H
#define ADJUSTMENTMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

class AdjustmentModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "adjustmentmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
public:
    TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) override;
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

#endif // ADJUSTMENTMODULEFACTORY_H
