#include "lambdamodulefactory.h"
#include "lambdamodule.h"

namespace LAMBDAMODULE
{

ZeraModules::VirtualModule* LambdaModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cLambdaModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void LambdaModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString LambdaModuleFactory::getFactoryName() const
{
    return QString(cLambdaModule::BaseModuleName).toLower();
}

}

