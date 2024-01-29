#include "referencemodulefactory.h"
#include "referencemodule.h"

namespace REFERENCEMODULE
{

ZeraModules::VirtualModule* ReferenceModuleFactory::createModule(int entityId, VeinEvent::StorageSystem* storagesystem, bool demo, int moduleNum)
{
    return new cReferenceModule(m_moduleGroupNumerator->requestModuleNum(moduleNum), entityId, storagesystem, demo);
}

void ReferenceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString ReferenceModuleFactory::getFactoryName() const
{
    return QString(cReferenceModule::BaseModuleName).toLower();
}

}

