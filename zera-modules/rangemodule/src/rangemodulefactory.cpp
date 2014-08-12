#include "rangemodulefactory.h"
#include "rangemodule.h"


ZeraModules::VirtualModule* RangeModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cRangeModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void RangeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    module->deleteLater();
}


QList<ZeraModules::VirtualModule *> RangeModuleFactory::listModules()
{
    return m_ModuleList;
}


QString RangeModuleFactory::getFactoryName()
{
    return QString("rangemodule");
}



