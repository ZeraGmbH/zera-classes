#ifndef LAMBDAMODULEFACTORY_H
#define LAMBDAMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace LAMBDAMODULE
{

class LambdaModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "lambdamodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    LambdaModuleFactory(){}
    ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QList<ZeraModules::VirtualModule *> listModules() const override;
    QString getFactoryName() const override;

private:
    QList<ZeraModules::VirtualModule*> m_ModuleList; // our list of modules
};

}
#endif // LAMBDAMODULEFACTORY_H
