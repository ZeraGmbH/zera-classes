#ifndef SAMPLEMODULEFACTORY_H
#define SAMPLEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace SAMPLEMODULE
{

class SampleModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "samplemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    SampleModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SAMPLEMODULEFACTORY_H
