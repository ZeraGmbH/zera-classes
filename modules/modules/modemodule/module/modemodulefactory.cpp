#include "modemodulefactory.h"
#include "modemodule.h"


namespace MODEMODULE
{

ZeraModules::VirtualModule* ModeModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cModeModule(moduleNum, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}


void ModeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString ModeModuleFactory::getFactoryName() const
{
    return QString(cModeModule::BaseModuleName).toLower();
}

}

