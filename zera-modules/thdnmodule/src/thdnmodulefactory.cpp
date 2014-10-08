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
    m_ModuleList.removeAll(module);
    module->deleteLater();
}


QList<ZeraModules::VirtualModule *> ThdnModuleFactory::listModules()
{
    return m_ModuleList;
}


QString ThdnModuleFactory::getFactoryName()
{
    return QString("thdnmodule");
}

}

