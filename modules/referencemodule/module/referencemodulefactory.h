#ifndef REFERENCEMODULEFACTORY_H
#define REFERENCEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace REFERENCEMODULE
{

class ReferenceModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "referencemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ReferenceModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // REFERENCEMODULEFACTORY_H
