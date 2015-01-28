#include "fftmodulefactory.h"
#include "fftmodule.h"

namespace FFTMODULE
{

ZeraModules::VirtualModule* FftModuleFactory::createModule(Zera::Proxy::cProxy* proxy, VeinPeer *peer, QObject *parent)
{
    ZeraModules::VirtualModule *module = new cFftModule(m_ModuleList.count()+1, proxy, peer, parent);
    m_ModuleList.append(module);
    return module;
}

void FftModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> FftModuleFactory::listModules()
{
    return m_ModuleList;
}


QString FftModuleFactory::getFactoryName()
{
    return QString("fftmodule");
}

}

