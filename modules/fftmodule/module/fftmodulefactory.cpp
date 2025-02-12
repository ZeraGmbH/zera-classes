#include "fftmodulefactory.h"
#include "fftmodule.h"
#include "cro_systemobserverfetchtask.h"

namespace FFTMODULE
{

TaskTemplatePtr FftModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

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

