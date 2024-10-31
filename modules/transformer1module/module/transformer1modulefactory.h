#ifndef TRANSFORMER1MODULEFACTORY_H
#define TRANSFORMER1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace TRANSFORMER1MODULE
{

class Transformer1ModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "transformer1module.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    Transformer1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // TRANSFORMER1MODULEFACTORY_H
