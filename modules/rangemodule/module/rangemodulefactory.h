#ifndef RANGEMODULEFACTORY_H
#define RANGEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace RANGEMODULE
{

class RangeModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "rangemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    RangeModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // RANGEMODULEFACTORY_H
