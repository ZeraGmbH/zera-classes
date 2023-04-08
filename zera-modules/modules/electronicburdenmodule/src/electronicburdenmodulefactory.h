#ifndef ELECTRONICBURDENMODULEFACTORY_H
#define ELECTRONICBURDENMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace ELECTRONICBURDENMODULE
{

class ElectronicBurdenModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "electronicburdenmodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    ElectronicBurdenModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, QObject* qObjParent = nullptr) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QList<ZeraModules::VirtualModule *> listModules() const override;;
    QString getFactoryName() const override;

private:
  QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}

#endif // ELECTRONICBURDENMODULEFACTORY_H
