#include "apimodulefactory.h"
#include "apimodule.h"

namespace APIMODULE
{

ZeraModules::VirtualModule *ApiModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cApiModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void ApiModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString ApiModuleFactory::getFactoryName() const
{
    return QString(cApiModule::BaseModuleName).toLower();
}

}
