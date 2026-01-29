#include "scpimodulefactory.h"
#include "scpimodule.h"

namespace SCPIMODULE
{

ZeraModules::VirtualModule *SCPIModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cSCPIModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void SCPIModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString SCPIModuleFactory::getFactoryName() const
{
    return QString(cSCPIModule::BaseModuleName).toLower();
}

}

