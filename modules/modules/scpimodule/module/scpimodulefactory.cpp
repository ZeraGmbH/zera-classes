#include "scpimodulefactory.h"
#include "scpimodule.h"

namespace SCPIMODULE
{

ZeraModules::VirtualModule *SCPIModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo)
{
    ZeraModules::VirtualModule *module = new cSCPIModule(m_ModuleList.count()+1, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void SCPIModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QList<ZeraModules::VirtualModule *> SCPIModuleFactory::listModules() const
{
    return m_ModuleList;
}


QString SCPIModuleFactory::getFactoryName() const
{
    return QString(cSCPIModule::BaseModuleName).toLower();
}

}

