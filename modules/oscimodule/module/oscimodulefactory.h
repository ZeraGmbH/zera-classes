#ifndef OSCIMODULEFACTORY_H
#define OSCIMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>

namespace OSCIMODULE
{

class OsciModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "oscimodule.json")
    Q_INTERFACES(AbstractModuleFactory)
  
public:
    OsciModuleFactory(){}
    ZeraModules::VirtualModule *createModule(ModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // OSCIMODULEFACTORY_H
