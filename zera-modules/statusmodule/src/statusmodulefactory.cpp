#include "statusmodulefactory.h"
#include "statusmodule.h"


namespace STATUSMODULE
{

ZeraModules::VirtualModule* StatusModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cStatusModule(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}


void StatusModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> StatusModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString StatusModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

