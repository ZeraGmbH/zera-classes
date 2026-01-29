#ifndef DOSAGEMODULEFACTORY_H
#define DOSAGEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace DOSAGEMODULE
{

class DosageModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "dosagemodule.json")
    Q_INTERFACES(AbstractModuleFactory)

public:
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // DOSAGEMODULEFACTORY_H
