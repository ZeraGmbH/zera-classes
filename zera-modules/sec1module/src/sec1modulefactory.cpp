#include "sec1modulefactory.h"
#include "sec1module.h"

namespace SEC1MODULE
{

ZeraModules::VirtualModule* Sec1ModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem *storagesystem, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cSec1Module(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void Sec1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Sec1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Sec1ModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

