#include "referencemodulefactory.h"
#include "referencemodule.h"

namespace REFERENCEMODULE
{

ZeraModules::VirtualModule* ReferenceModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cReferenceModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
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

