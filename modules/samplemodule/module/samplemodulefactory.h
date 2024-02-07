#ifndef SAMPLEMODULEFACTORY_H
#define SAMPLEMODULEFACTORY_H

#include <abstractmodulefactory.h>
#include <virtualmodule.h>
#include <QList>
#include <QtPlugin>

namespace SAMPLEMODULE
{

class SampleModuleFactory : public QObject, public MeasurementModuleFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MeasurementModuleFactory_iid FILE "samplemodule.json")
    Q_INTERFACES(MeasurementModuleFactory)
  
public:
    SampleModuleFactory(){}
    ZeraModules::VirtualModule *createModule(MeasurementModuleFactoryParam moduleParam) override;
    void destroyModule(ZeraModules::VirtualModule *module) override;
    QString getFactoryName() const override;
};

}

#endif // SAMPLEMODULEFACTORY_H
