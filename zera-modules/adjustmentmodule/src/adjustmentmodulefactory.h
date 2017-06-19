#ifndef ADJUSTMENTMODULEFACTORY_H
#define ADJUSTMENTMODULEFACTORY_H

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


namespace ADJUSTMENTMODULE
{

class AdjustmentModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "adjustmentmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    AdjustmentModuleFactory(){}
    ZeraModules::VirtualModule *createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    void destroyModule(ZeraModules::VirtualModule *module); //override;
    QList<ZeraModules::VirtualModule *> listModules() const; //override;
    QString getFactoryName() const; //override;

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}
#endif // ADJUSTMENTMODULEFACTORY_H
