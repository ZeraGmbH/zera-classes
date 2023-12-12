#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include "virtualmodule.h"
#include <QtPlugin>
#include <QList>

namespace VeinEvent
{
    class StorageSystem;
}

class MeasurementModuleFactory
{
public:
    virtual ~MeasurementModuleFactory() = default;
    virtual ZeraModules::VirtualModule *createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum) = 0;
    virtual void destroyModule(ZeraModules::VirtualModule *module) = 0;
    virtual QList<ZeraModules::VirtualModule*> listModules() const = 0;
    virtual QString getFactoryName() const = 0;
};

#define MeasurementModuleFactory_iid "org.qt-project.Qt.Examples.Test/1.0"
Q_DECLARE_INTERFACE(MeasurementModuleFactory, MeasurementModuleFactory_iid)

#endif // MODULEINTERFACE_H
