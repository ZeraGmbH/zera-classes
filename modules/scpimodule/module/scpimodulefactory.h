#ifndef SCPIMODULEFACTORY_H
#define SCPIMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace SCPIMODULE
{

class SCPIModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "scpimodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    SCPIModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SCPIMODULEFACTORY_H
