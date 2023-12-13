#include "spm1modulefactory.h"
#include "spm1module.h"

namespace SPM1MODULE
{

ZeraModules::VirtualModule* Spm1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    return new cSpm1Module(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void Spm1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Spm1ModuleFactory::getFactoryName() const
{
    return QString(cSpm1Module::BaseModuleName).toLower();
}

}

