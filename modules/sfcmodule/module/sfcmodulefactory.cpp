#include "sfcmodulefactory.h"
#include "sfcmodule.h"

namespace SFCMODULE
{

ZeraModules::VirtualModule *SfcModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cSfcModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void SfcModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString SfcModuleFactory::getFactoryName() const
{
    return QString(cSfcModule::BaseModuleName).toLower();
}

}
