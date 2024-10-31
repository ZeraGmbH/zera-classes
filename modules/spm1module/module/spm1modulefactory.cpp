#include "spm1modulefactory.h"
#include "spm1module.h"

namespace SPM1MODULE
{

ZeraModules::VirtualModule* Spm1ModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cSpm1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Spm1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Spm1ModuleFactory::getFactoryName() const
{
    return QString(cSpm1Module::BaseModuleName).toLower();
}

}

