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
    ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}
#endif // LAMBDAMODULEFACTORY_H
