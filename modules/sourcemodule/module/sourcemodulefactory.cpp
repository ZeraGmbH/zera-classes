#include "sourcemodulefactory.h"
#include "sourcemodule.h"

ZeraModules::VirtualModule* SourceModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new SourceModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}


void SourceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString SourceModuleFactory::getFactoryName() const
{
    return QString(SourceModule::BaseModuleName).toLower();
}

