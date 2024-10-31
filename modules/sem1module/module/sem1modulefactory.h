#ifndef SEM1MODULEFACTORY_H
#define SEM1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace SEM1MODULE
{

class Sem1ModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sem1module.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    Sem1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SEM1MODULEFACTORY_H
