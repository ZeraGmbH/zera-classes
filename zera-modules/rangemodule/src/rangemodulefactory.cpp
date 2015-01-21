#include "rangemodulefactory.h"
#include "rangemodule.h"

namespace RANGEMODULE
{

ZeraModules::VirtualModule* RangeModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cRangeModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void RangeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    module2Delete = module;
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), this, SLOT(deleteModule()));
    module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> RangeModuleFactory::listModules()
{
    return m_ModuleList;
}


QString RangeModuleFactory::getFactoryName()
{
    return QString("rangemodule");
}


void RangeModuleFactory::deleteModule()
{
    emit module2Delete->moduleDeactivated();
    delete module2Delete;
}

}

