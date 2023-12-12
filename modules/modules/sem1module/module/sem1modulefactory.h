#ifndef SEM1MODULEFACTORY_H
#define SEM1MODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace SEM1MODULE
{

class Sem1ModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sem1module.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    Sem1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum);
    void destroyModule(ZeraModules::VirtualModule *module); //override;
    QString getFactoryName() const; //override;

private:
    QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}

#endif // SEM1MODULEFACTORY_H
