#include "samplemodulefactory.h"
#include "samplemodule.h"
#include "cro_systemobserverfetchtask.h"

namespace SAMPLEMODULE
{

TaskTemplatePtr SampleModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

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

