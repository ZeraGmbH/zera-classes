#include "power3modulefactory.h"
#include "power3module.h"

namespace POWER3MODULE
{

ZeraModules::VirtualModule* Power3ModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cPower3Module(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void Power3ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Power3ModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString Power3ModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

