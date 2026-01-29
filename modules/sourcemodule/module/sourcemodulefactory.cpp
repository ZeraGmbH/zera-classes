#include "sourcemodulefactory.h"
#include "sourcemodule.h"

ZeraModules::VirtualModule* SourceModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new SourceModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void SourceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString SourceModuleFactory::getFactoryName() const
{
    return QString(SourceModule::BaseModuleName).toLower();
}

