#ifndef POWER3MODULEFACTORY_H
#define POWER3MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace POWER3MODULE
{

class Power3ModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "power3module.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    Power3ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // POWERMODULEFACTORY_H
