#include "transformer1modulefactory.h"
#include "transformer1module.h"

namespace TRANSFORMER1MODULE
{

ZeraModules::VirtualModule* Transformer1ModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cTransformer1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Transformer1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Transformer1ModuleFactory::getFactoryName() const
{
    return QString(cTransformer1Module::BaseModuleName).toLower();
}

}

