#ifndef THDNMODULEFACTORY_H
#define THDNMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace THDNMODULE
{

class ThdnModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "thdnmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ThdnModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // THDNMODULEFACTORY_H
