#ifndef DFTMODULEFACTORY_H
#define DFTMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace DFTMODULE
{

class DftModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "dftmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    DftModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QList<ZeraModules::VirtualModule *> listModules() const override;
    QString getFactoryName() const override;

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}
#endif // DFTMODULEFACTORY_H
