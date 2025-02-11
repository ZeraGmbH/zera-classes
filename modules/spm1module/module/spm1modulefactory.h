#ifndef SPM1MODULEFACTORY_H
#define SPM1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace SPM1MODULE
{

class Spm1ModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "spm1module.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    Spm1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SPM1MODULEFACTORY_H
