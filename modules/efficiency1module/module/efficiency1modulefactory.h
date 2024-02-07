#ifndef EFFICIENCY1MODULEFACTORY_H
#define EFFICIENCY1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace EFFICIENCY1MODULE
{

class Efficiency1ModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "efficiency1module.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    Efficiency1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // POWERMODULEFACTORY_H
