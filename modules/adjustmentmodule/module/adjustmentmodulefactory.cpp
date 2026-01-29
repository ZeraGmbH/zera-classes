#include "adjustmentmodulefactory.h"
#include "adjustmentmodule.h"
#include "cro_systemobserverfetchtask.h"

TaskTemplatePtr AdjustmentModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* AdjustmentModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cAdjustmentModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void AdjustmentModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString AdjustmentModuleFactory::getFactoryName() const
{
    return QString(cAdjustmentModule::BaseModuleName).toLower();
}
