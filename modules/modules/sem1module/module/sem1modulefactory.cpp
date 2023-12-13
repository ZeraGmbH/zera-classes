#include "sem1modulefactory.h"
#include "sem1module.h"

namespace SEM1MODULE
{

ZeraModules::VirtualModule* Sem1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    return new cSem1Module(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void Sem1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Sem1ModuleFactory::getFactoryName() const
{
    return QString(cSem1Module::BaseModuleName).toLower();
}

}

