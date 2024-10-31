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
    module->startDestroy();
}

QString ThdnModuleFactory::getFactoryName() const
{
    return QString(cThdnModule::BaseModuleName).toLower();
}

}

