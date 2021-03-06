#include "sem1modulefactory.h"
#include "sem1module.h"

namespace SEM1MODULE
{

ZeraModules::VirtualModule* Sem1ModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem *storagesystem, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cSem1Module(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void Sem1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Sem1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Sem1ModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

