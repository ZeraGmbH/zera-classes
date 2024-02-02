#ifndef THDNMODULEFACTORY_H
#define THDNMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace THDNMODULE
{

class ThdnModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "thdnmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    ThdnModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // THDNMODULEFACTORY_H
