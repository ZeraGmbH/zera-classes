#include "sec1modulefactory.h"
#include "sec1module.h"

namespace SEC1MODULE
{

ZeraModules::VirtualModule* Sec1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    return new cSec1Module(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void Sec1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Sec1ModuleFactory::getFactoryName() const
{
    return QString(cSec1Module::BaseModuleName).toLower();
}

}

