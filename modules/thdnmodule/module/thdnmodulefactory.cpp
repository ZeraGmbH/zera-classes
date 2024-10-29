#include "thdnmodulefactory.h"
#include "thdnmodule.h"

namespace THDNMODULE
{

ZeraModules::VirtualModule* ThdnModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cThdnModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void ThdnModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    connect(module, &ZeraModules::VirtualModule::deactivationReady, module, &ZeraModules::VirtualModule::moduleDeactivated);
    if (!module->m_DeactivationMachine.isRunning())
        module->m_DeactivationMachine.start();
}


QString ThdnModuleFactory::getFactoryName() const
{
    return QString(cThdnModule::BaseModuleName).toLower();
}

}

