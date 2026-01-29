#ifndef SOURCEMODULEFACTORY_H
#define SOURCEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

class SourceModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sourcemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

#endif // SOURCEMODULEFACTORY_H
