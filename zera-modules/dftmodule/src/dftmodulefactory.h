#ifndef DFTMODULEFACTORY_H
#define DFTMODULEFACTORY_H

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


namespace DFTMODULE
{

class DftModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "dftmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    DftModuleFactory(){}
    ZeraModules::VirtualModule *createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    void destroyModule(ZeraModules::VirtualModule *module); //override;
    QList<ZeraModules::VirtualModule *> listModules() const; //override;
    QString getFactoryName() const; //override;

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}
#endif // DFTMODULEFACTORY_H
