#include "adjustmentmodulefactory.h"
#include "adjustmentmodule.h"

ZeraModules::VirtualModule* AdjustmentModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cAdjustmentModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void AdjustmentModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString AdjustmentModuleFactory::getFactoryName() const
{
    return QString(cAdjustmentModule::BaseModuleName).toLower();
}
