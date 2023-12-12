#include "power3modulefactory.h"
#include "power3module.h"

namespace POWER3MODULE
{

ZeraModules::VirtualModule* Power3ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cPower3Module(moduleNum, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Power3ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Power3ModuleFactory::getFactoryName() const
{
    return QString(cPower3Module::BaseModuleName).toLower();
}

}

