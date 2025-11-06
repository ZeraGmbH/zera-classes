#ifndef HOTPLUGCONTROLSMODULEFACTORY_H
#define HOTPLUGCONTROLSMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace HOTPLUGCONTROLSMODULE
{

class HotplugControlsModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "hotplugcontrolsmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // HOTPLUGCONTROLSMODULEFACTORY_H
