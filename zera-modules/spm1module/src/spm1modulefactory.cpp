#include "spm1modulefactory.h"
#include "spm1module.h"

namespace SPM1MODULE
{

ZeraModules::VirtualModule* Spm1ModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem *storagesystem, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cSpm1Module(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void Spm1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Spm1ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Spm1ModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

