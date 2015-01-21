#include "samplemodulefactory.h"
#include "samplemodule.h"

namespace SAMPLEMODULE
{

ZeraModules::VirtualModule* SampleModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cSampleModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void SampleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    module2Delete = module;
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), this, SLOT(deleteModule()));
    module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> SampleModuleFactory::listModules()
{
    return m_ModuleList;
}


QString SampleModuleFactory::getFactoryName()
{
    return QString("samplemodule");
}


void SampleModuleFactory::deleteModule()
{
    emit module2Delete->moduleDeactivated();
    delete module2Delete;
}

}

