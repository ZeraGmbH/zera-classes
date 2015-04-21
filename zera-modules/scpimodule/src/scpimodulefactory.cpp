#include "scpimodulefactory.h"
#include "scpimodule.h"

namespace SCPIMODULE
{

ZeraModules::VirtualModule* SCPIModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cSCPIModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void SCPIModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> SCPIModuleFactory::listModules()
{
    return m_ModuleList;
}


QString SCPIModuleFactory::getFactoryName()
{
    return QString("scpimodule");
}

}

