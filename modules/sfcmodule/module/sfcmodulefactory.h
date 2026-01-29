#ifndef SFCMODULEFACTORY_H
#define SFCMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace SFCMODULE
{

class SfcModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sfcmodule.json")
    Q_INTERFACES(AbstractModuleFactory)

public:
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // SFCMODULEFACTORY_H
