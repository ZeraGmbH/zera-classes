#ifndef MODEMODULEFACTORY_H
#define MODEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace MODEMODULE
{

class ModeModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "modemodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ModeModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // MODEMODULEFACTORY_H
