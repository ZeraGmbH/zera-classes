#ifndef ELECTRONICBURDENMODULEFACTORY_H
#define ELECTRONICBURDENMODULEFACTORY_H

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

namespace ELECTRONICBURDENMODULE
{

class ElectronicBurdenModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "electronicburdenmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    ElectronicBurdenModuleFactory(){}
    ZeraModules::VirtualModule *createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* qObjParent = 0) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QList<ZeraModules::VirtualModule *> listModules() const override;;
    QString getFactoryName() const override;

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}

#endif // ELECTRONICBURDENMODULEFACTORY_H
