#ifndef STATUSMODULEFACTORY_H
#define STATUSMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace STATUSMODULE
{

class StatusModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "statusmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    StatusModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QList<ZeraModules::VirtualModule *> listModules() const override;
    QString getFactoryName() const override;

private:
    QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}

#endif // STATUSMODULEFACTORY_H
