#ifndef POWER3MODULEFACTORY_H
#define POWER3MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace POWER3MODULE
{

class Power3ModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "power3module.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    Power3ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // POWERMODULEFACTORY_H
