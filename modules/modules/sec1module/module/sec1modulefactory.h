#ifndef SEC1MODULEFACTORY_H
#define SEC1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace SEC1MODULE
{

class Sec1ModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sec1module.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    Sec1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum);
    void destroyModule(ZeraModules::VirtualModule *module); //override;
    QList<ZeraModules::VirtualModule *> listModules() const; //override;
    QString getFactoryName() const; //override;

private:
    QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}

#endif // SEC1MODULEFACTORY_H
