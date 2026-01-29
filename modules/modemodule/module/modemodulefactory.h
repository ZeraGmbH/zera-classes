#ifndef MODEMODULEFACTORY_H
#define MODEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace MODEMODULE
{

class ModeModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "modemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // MODEMODULEFACTORY_H
