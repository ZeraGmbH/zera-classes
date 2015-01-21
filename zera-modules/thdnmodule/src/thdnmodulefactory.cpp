#include "thdnmodulefactory.h"
#include "thdnmodule.h"

namespace THDNMODULE
{

ZeraModules::VirtualModule* ThdnModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cThdnModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void ThdnModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    module2Delete = module;
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), this, SLOT(deleteModule()));
    module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> ThdnModuleFactory::listModules()
{
    return m_ModuleList;
}


QString ThdnModuleFactory::getFactoryName()
{
    return QString("thdnmodule");
}


void ThdnModuleFactory::deleteModule()
{
    emit module2Delete->moduleDeactivated();
    delete module2Delete;
}

}

