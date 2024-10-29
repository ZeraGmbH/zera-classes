#ifndef SEC1MODULEFACTORY_H
#define SEC1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace SEC1MODULE
{

class Sec1ModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sec1module.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    Sec1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SEC1MODULEFACTORY_H
