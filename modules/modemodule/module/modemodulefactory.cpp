#include "modemodulefactory.h"
#include "modemodule.h"

namespace MODEMODULE
{

ZeraModules::VirtualModule* ModeModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cModeModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void ModeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString ModeModuleFactory::getFactoryName() const
{
    return QString(cModeModule::BaseModuleName).toLower();
}

}

