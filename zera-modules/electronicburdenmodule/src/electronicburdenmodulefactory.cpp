#include "electronicburdenmodulefactory.h"
#include "electronicburdenmodule.h"


namespace ELECTRONICBURDENMODULE
{

ZeraModules::VirtualModule* ElectronicBurdenModuleFactory::createModule(Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent)
{
    ZeraModules::VirtualModule *module = new ElectronicBurdenModule(m_ModuleList.count()+1, proxy, entityId, storagesystem, parent);
    m_ModuleList.append(module);
    return module;
}


void ElectronicBurdenModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> ElectronicBurdenModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString ElectronicBurdenModuleFactory::getFactoryName() const
{
    return QString(BaseModuleName).toLower();
}

}

