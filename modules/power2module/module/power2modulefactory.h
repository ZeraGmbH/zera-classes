#ifndef POWER2MODULEFACTORY_H
#define POWER2MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace POWER2MODULE
{

class Power2ModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "power2module.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    Power2ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // POWERMODULEFACTORY_H
