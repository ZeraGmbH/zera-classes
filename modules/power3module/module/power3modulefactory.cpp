#include "power3modulefactory.h"
#include "power3module.h"

namespace POWER3MODULE
{

ZeraModules::VirtualModule* Power3ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cPower3Module(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void Power3ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Power3ModuleFactory::getFactoryName() const
{
    return QString(cPower3Module::BaseModuleName).toLower();
}

}

