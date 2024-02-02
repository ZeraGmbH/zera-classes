#include "efficiency1modulefactory.h"
#include "efficiency1module.h"

namespace EFFICIENCY1MODULE
{

ZeraModules::VirtualModule* Efficiency1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cEfficiency1Module(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void Efficiency1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Efficiency1ModuleFactory::getFactoryName() const
{
    return QString(cEfficiency1Module::BaseModuleName).toLower();
}

}

