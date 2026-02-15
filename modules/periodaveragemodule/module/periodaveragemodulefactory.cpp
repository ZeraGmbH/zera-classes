#include "periodaveragemodulefactory.h"
#include "periodaveragemodule.h"
#include "cro_systemobserverfetchtask.h"

namespace PERIODAVERAGEMODULE
{

TaskTemplatePtr PeriodAverageModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* PeriodAverageModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new PeriodAverageModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void PeriodAverageModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString PeriodAverageModuleFactory::getFactoryName() const
{
    return QString(PeriodAverageModule::BaseModuleName).toLower();
}

}

