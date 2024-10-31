#include "rangemodulefactory.h"
#include "rangemodule.h"

namespace RANGEMODULE
{

ZeraModules::VirtualModule* RangeModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cRangeModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void RangeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString RangeModuleFactory::getFactoryName() const
{
    return QString(cRangeModule::BaseModuleName).toLower();
}

}

