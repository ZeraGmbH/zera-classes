#include "rangemodulefactory.h"
#include "rangemodule.h"
#include "cro_systemobserverfetchtask.h"

namespace RANGEMODULE
{

TaskTemplatePtr RangeModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* RangeModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cRangeModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void RangeModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString RangeModuleFactory::getFactoryName() const
{
    return QString(cRangeModule::BaseModuleName).toLower();
}

}

