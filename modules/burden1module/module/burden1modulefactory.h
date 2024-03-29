#ifndef BURDEN1MODULEFACTORY_H
#define BURDEN1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace BURDEN1MODULE
{

class Burden1ModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "burden1module.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    Burden1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // BURDEN1MODULEFACTORY_H
