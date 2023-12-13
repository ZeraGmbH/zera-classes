#include "scpimodulefactory.h"
#include "scpimodule.h"

namespace SCPIMODULE
{

ZeraModules::VirtualModule *SCPIModuleFactory::createModule(int entityId, VeinEvent::StorageSystem *storagesystem, bool demo, int moduleNum)
{
    return new cSCPIModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void SCPIModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString SCPIModuleFactory::getFactoryName() const
{
    return QString(cSCPIModule::BaseModuleName).toLower();
}

}

