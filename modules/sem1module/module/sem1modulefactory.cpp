#include "sem1modulefactory.h"
#include "sem1module.h"

namespace SEM1MODULE
{

static const char* ModuleName = "SEM1Module";

ZeraModules::VirtualModule* Sem1ModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cSem1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()),
                           "This module provides a configurable energy error calculator",
                           ModuleName,
                           "EM01");
}

void Sem1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Sem1ModuleFactory::getFactoryName() const
{
    return QString(ModuleName).toLower();
}

}

