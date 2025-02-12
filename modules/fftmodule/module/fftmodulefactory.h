#ifndef FFTMODULEFACTORY_H
#define FFTMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace FFTMODULE
{

class FftModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "fftmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    TaskTemplatePtr getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData) override;
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // FFTMODULEFACTORY_H
