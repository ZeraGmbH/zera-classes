#include "sec1modulefactory.h"
#include "sec1module.h"

namespace SEC1MODULE
{

ZeraModules::VirtualModule* Sec1ModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cSec1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Sec1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Sec1ModuleFactory::getFactoryName() const
{
    return QString(cSec1Module::BaseModuleName).toLower();
}

}

