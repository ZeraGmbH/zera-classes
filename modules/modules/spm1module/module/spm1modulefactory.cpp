#include "spm1modulefactory.h"
#include "spm1module.h"

namespace SPM1MODULE
{

ZeraModules::VirtualModule* Spm1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    ZeraModules::VirtualModule *module = new cSpm1Module(moduleNum, entityId, storagesystem, demo);
    m_ModuleList.append(module);
    return module;
}

void Spm1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_ModuleList.removeAll(module);
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Spm1ModuleFactory::getFactoryName() const
{
    return QString(cSpm1Module::BaseModuleName).toLower();
}

}

