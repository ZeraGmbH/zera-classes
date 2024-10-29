#include "sourcemodulefactory.h"
#include "sourcemodule.h"

ZeraModules::VirtualModule* SourceModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new SourceModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
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

