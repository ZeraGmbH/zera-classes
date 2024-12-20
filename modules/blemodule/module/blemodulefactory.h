#ifndef BLEMODULEFACTORY_H
#define BLEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace BLEMODULE
{

class BleModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "blemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    BleModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // BLEMODULEFACTORY_H
