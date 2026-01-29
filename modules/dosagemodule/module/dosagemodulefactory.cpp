#include "dosagemodulefactory.h"
#include "dosagemodule.h"

namespace DOSAGEMODULE
{

ZeraModules::VirtualModule *DosageModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cDosageModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void DosageModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString DosageModuleFactory::getFactoryName() const
{
    return QString(cDosageModule::BaseModuleName).toLower();
}

}
