#ifndef SOURCEMODULEFACTORY_H
#define SOURCEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

class SourceModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "sourcemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    SourceModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

#endif // SOURCEMODULEFACTORY_H
