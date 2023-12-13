#ifndef RMSMODULEFACTORY_H
#define RMSMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace RMSMODULE
{

class RmsModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "rmsmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    RmsModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // RMSMODULEFACTORY_H
