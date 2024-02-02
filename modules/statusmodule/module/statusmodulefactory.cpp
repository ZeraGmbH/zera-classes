#include "statusmodulefactory.h"
#include "statusmodule.h"


namespace STATUSMODULE
{

ZeraModules::VirtualModule* StatusModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cStatusModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}


void StatusModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString StatusModuleFactory::getFactoryName() const
{
    return QString(cStatusModule::BaseModuleName).toLower();
}

}

