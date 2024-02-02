#include "transformer1modulefactory.h"
#include "transformer1module.h"

namespace TRANSFORMER1MODULE
{

ZeraModules::VirtualModule* Transformer1ModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cTransformer1Module(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void Transformer1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString Transformer1ModuleFactory::getFactoryName() const
{
    return QString(cTransformer1Module::BaseModuleName).toLower();
}

}

