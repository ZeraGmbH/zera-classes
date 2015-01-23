#include <veinpeer.h>
#include <QDebug>

#include "rmsmodulefactory.h"
#include "rmsmodule.h"

namespace RMSMODULE
{

ZeraModules::VirtualModule* RmsModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    qDebug() << peer->getEntityList().count();
    ZeraModules::VirtualModule *module = new cRmsModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}


void RmsModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    module2Delete = module;
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), this, SLOT(deleteModule()));
    module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> RmsModuleFactory::listModules()
{
    return m_ModuleList;
}


QString RmsModuleFactory::getFactoryName()
{
    return QString("rmsmodule");
}


void RmsModuleFactory::deleteModule()
{
    emit module2Delete->moduleDeactivated();
}

}

