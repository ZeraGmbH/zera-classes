#include "lamdamodulefactory.h"
#include "lamdamodule.h"

namespace LAMDAMODULE
{

ZeraModules::VirtualModule* LamdaModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new cLamdaModule(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}

void LamdaModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, SIGNAL(deactivationReady()), module, SIGNAL(moduleDeactivated()));
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> LamdaModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString LamdaModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

