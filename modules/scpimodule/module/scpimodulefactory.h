#ifndef SCPIMODULEFACTORY_H
#define SCPIMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace SCPIMODULE
{


class SCPIModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "scpimodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    SCPIModuleFactory(){}
    ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SCPIMODULEFACTORY_H
