#include "fftmodulefactory.h"
#include "fftmodule.h"

namespace FFTMODULE
{

ZeraModules::VirtualModule* FftModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cFftModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void FftModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString FftModuleFactory::getFactoryName() const
{
    return QString(cFftModule::BaseModuleName).toLower();
}

}

