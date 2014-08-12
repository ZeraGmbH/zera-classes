#include "rmsmodulefactory.h"
#include "rmsmodule.h"

namespace RMSMODULE
{

ZeraModules::VirtualModule* RmsModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cRmsModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void RmsModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    module->deleteLater();
}


QList<ZeraModules::VirtualModule *> RmsModuleFactory::listModules()
{
    return m_ModuleList;
}


QString RmsModuleFactory::getFactoryName()
{
    return QString("rmsmodule");
}

}

