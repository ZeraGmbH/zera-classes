#include "power1modulefactory.h"
#include "power1module.h"

namespace POWER1MODULE
{

ZeraModules::VirtualModule* Power1ModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cPower1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Power1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Power1ModuleFactory::getFactoryName() const
{
    return QString(cPower1Module::BaseModuleName).toLower();
}

}

