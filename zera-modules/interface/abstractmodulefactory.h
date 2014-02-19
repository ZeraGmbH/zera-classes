#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H
#include <QtPlugin>

#include "virtualmodule.h"

#include <QList>

namespace Zera
{
namespace Proxi
{
    class cProxi;
}
}

class VeinPeer;

class MeasurementModuleFactory
{
public:
  virtual ~MeasurementModuleFactory() {}
  virtual ZeraModules::VirtualModule *createModule(Zera::Proxi::cProxi* proxi, VeinPeer* peer, QObject* qObjParent = 0)=0;
  virtual void destroyModule(ZeraModules::VirtualModule *module)=0;
  virtual QList<ZeraModules::VirtualModule*> listModules()=0;
  virtual QString getFactoryName() =0;
};

#define MeasurementModuleFactory_iid "org.qt-project.Qt.Examples.Test/1.0"
Q_DECLARE_INTERFACE(MeasurementModuleFactory, MeasurementModuleFactory_iid)

#endif // MODULEINTERFACE_H
