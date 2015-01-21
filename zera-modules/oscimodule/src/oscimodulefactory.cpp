#include "oscimodulefactory.h"
#include "oscimodule.h"

namespace OSCIMODULE
{

ZeraModules::VirtualModule* OsciModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cOsciModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void OsciModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    module2Delete = module;
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), this, SLOT(deleteModule()));
    module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> OsciModuleFactory::listModules()
{
    return m_ModuleList;
}


QString OsciModuleFactory::getFactoryName()
{
    return QString("oscimodule");
}


void OsciModuleFactory::deleteModule()
{
    emit module2Delete->moduleDeactivated();
    delete module2Delete;
}

}

