#include "rmsmodulefactory.h"
#include "rmsmodule.h"
#include "cro_systemobserverfetchtask.h"

namespace RMSMODULE
{

TaskTemplatePtr RmsModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* RmsModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cRmsModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void RmsModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString RmsModuleFactory::getFactoryName() const
{
    return QString(cRmsModule::BaseModuleName).toLower();
}

}

