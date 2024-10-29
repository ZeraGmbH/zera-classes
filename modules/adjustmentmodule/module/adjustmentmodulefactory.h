#ifndef ADJUSTMENTMODULEFACTORY_H
#define ADJUSTMENTMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

class AdjustmentModuleFactory : public QObject, public AbstractModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "adjustmentmodule.json")
    Q_INTERFACES(AbstractModuleFactory)
public:
    AdjustmentModuleFactory(){}
    ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

#endif // ADJUSTMENTMODULEFACTORY_H
