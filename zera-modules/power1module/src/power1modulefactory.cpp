#include "power1modulefactory.h"
#include "power1module.h"

namespace POWER1MODULE
{

ZeraModules::VirtualModule* Power1ModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cPower1Module(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void Power1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    module2Delete = module;
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), this, SLOT(deleteModule()));
    module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> Power1ModuleFactory::listModules()
{
    return m_ModuleList;
}


QString Power1ModuleFactory::getFactoryName()
{
    return QString("power1module");
}


void Power1ModuleFactory::deleteModule()
{
    emit module2Delete->moduleDeactivated();
}

}

