#include "referencemodulefactory.h"
#include "referencemodule.h"

namespace REFERENCEMODULE
{

ZeraModules::VirtualModule* ReferenceModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cReferenceModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void ReferenceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    module2Delete = module;
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), this, SLOT(deleteModule()));
    module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> ReferenceModuleFactory::listModules()
{
    return m_ModuleList;
}


QString ReferenceModuleFactory::getFactoryName()
{
    return QString("referencemodule");
}

void ReferenceModuleFactory::deleteModule()
{
    emit module2Delete->moduleDeactivated();
}

}

