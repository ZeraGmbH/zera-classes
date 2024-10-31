#include "statusmodulefactory.h"
#include "statusmodule.h"

namespace STATUSMODULE
{

ZeraModules::VirtualModule* StatusModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cStatusModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void StatusModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString StatusModuleFactory::getFactoryName() const
{
    return QString(cStatusModule::BaseModuleName).toLower();
}

}

