#include <QDebug>

#include "rmsmodulefactory.h"
#include "rmsmodule.h"

namespace RMSMODULE
{

ZeraModules::VirtualModule* RmsModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cRmsModule(m_ModuleList.count()+1, Zera::Proxy::cProxy::getInstance(), entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}


void RmsModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> RmsModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString RmsModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

