#include "modemodulefactory.h"
#include "modemodule.h"

namespace MODEMODULE
{

ZeraModules::VirtualModule* ModeModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cModeModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void ModeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> ModeModuleFactory::listModules()
{
    return m_ModuleList;
}


QString ModeModuleFactory::getFactoryName()
{
    return QString("modemodule");
}

}

