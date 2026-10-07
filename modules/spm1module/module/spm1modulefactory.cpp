#include "spm1modulefactory.h"
#include "spm1module.h"

namespace SPM1MODULE
{

static const char* ModuleName = "SPM1Module";

ZeraModules::VirtualModule* Spm1ModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cSpm1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()),
                           "This module provides a configurable power error calculator",
                           ModuleName,
                           "PM01");
}

void Spm1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Spm1ModuleFactory::getFactoryName() const
{
    return QString(ModuleName).toLower();
}

}

