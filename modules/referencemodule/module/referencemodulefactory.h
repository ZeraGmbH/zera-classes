#ifndef REFERENCEMODULEFACTORY_H
#define REFERENCEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace REFERENCEMODULE
{

class ReferenceModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "referencemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) override;
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // REFERENCEMODULEFACTORY_H
