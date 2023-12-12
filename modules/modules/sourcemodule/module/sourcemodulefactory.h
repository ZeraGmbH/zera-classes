#ifndef SOURCEMODULEFACTORY_H
#define SOURCEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

class SourceModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sourcemodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    SourceModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

#endif // SOURCEMODULEFACTORY_H
