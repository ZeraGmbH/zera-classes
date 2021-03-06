#include "burden1modulefactory.h"
#include "burden1module.h"

namespace BURDEN1MODULE
{

ZeraModules::VirtualModule* Burden1ModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cBurden1Module(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void Burden1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Burden1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Burden1ModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

