#ifndef THDNMODULEFACTORY_H
#define THDNMODULEFACTORY_H

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

class VeinPeer;

namespace THDNMODULE
{

class ThdnModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "thdnmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    ThdnModuleFactory(){}
    ZeraModules::VirtualModule *createModule(Zera::Proxy::cProxy* proxy, VeinPeer* peer, QObject* parent = 0);
    void destroyModule(ZeraModules::VirtualModule *module); //override;
    QList<ZeraModules::VirtualModule *> listModules(); //override;
    QString getFactoryName(); //override;

protected slots:
  virtual void deleteModule();

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
  ZeraModules::VirtualModule* module2Delete;
};

}
#endif // THDNMODULEFACTORY_H
