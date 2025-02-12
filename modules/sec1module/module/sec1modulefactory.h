#ifndef SEC1MODULEFACTORY_H
#define SEC1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace SEC1MODULE
{

class Sec1ModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sec1module.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SEC1MODULEFACTORY_H
