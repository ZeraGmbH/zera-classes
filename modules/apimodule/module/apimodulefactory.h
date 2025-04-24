#ifndef APIMODULEFACTORY_H
#define APIMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace APIMODULE
{

class ApiModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "apimodule.json")
    Q_INTERFACES(AbstractModuleFactory)

public:
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // APIMODULEFACTORY_H
