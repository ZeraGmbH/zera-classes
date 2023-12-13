#include "modemodulefactory.h"
#include "modemodule.h"


namespace MODEMODULE
{

ZeraModules::VirtualModule* ModeModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cModeModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}


void ModeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString ModeModuleFactory::getFactoryName() const
{
    return QString(cModeModule::BaseModuleName).toLower();
}

}

