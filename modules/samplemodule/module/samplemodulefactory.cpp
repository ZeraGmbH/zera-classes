#include "samplemodulefactory.h"
#include "samplemodule.h"

namespace SAMPLEMODULE
{

ZeraModules::VirtualModule* SampleModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cSampleModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void SampleModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString SampleModuleFactory::getFactoryName() const
{
    return QString(cSampleModule::BaseModuleName).toLower();
}

}

