#ifndef SPM1MODULEFACTORY_H
#define SPM1MODULEFACTORY_H

#include <QObject>
#include <QList>
#include <QtPlugin>
#include <abstractmodulefactory.h>
#include <virtualmodule.h>


namespace Zera
{
namespace Proxy
{
    class cProxy;
}
}

namespace VeinEvent
{
    class StorageSystem;
}

namespace SPM1MODULE
{

class Spm1ModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "spm1module.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    Spm1ModuleFactory(){}
    ZeraModules::VirtualModule *createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    void destroyModule(ZeraModules::VirtualModule *module); //override;
    QList<ZeraModules::VirtualModule *> listModules() const; //override;
    QString getFactoryName() const; //override;

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}

#endif // SPM1MODULEFACTORY_H
