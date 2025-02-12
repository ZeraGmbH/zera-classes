#ifndef STATUSMODULEFACTORY_H
#define STATUSMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace STATUSMODULE
{

class StatusModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "statusmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // STATUSMODULEFACTORY_H
