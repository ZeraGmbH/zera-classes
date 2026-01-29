#include "recordermodulefactory.h"
#include "recordermodule.h"

ZeraModules::VirtualModule *RecorderModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new RecorderModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void RecorderModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString RecorderModuleFactory::getFactoryName() const
{
    return QString(RecorderModule::BaseModuleName).toLower();
}
