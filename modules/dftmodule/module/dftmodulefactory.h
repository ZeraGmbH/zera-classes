#ifndef DFTMODULEFACTORY_H
#define DFTMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace DFTMODULE
{

class DftModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "dftmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    DftModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // DFTMODULEFACTORY_H
