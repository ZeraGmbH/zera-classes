#ifndef RECORDERMODULEFACTORY_H
#define RECORDERMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

class RecorderModuleFactory : public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "recordermodule.json")
    Q_INTERFACES(AbstractModuleFactory)

public:
    ZeraModules::VirtualModule *createModule(const ModuleFactoryParam &moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

#endif // RECORDERMODULEFACTORY_H
