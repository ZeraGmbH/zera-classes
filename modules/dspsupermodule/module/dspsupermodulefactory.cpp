#include "dspsupermodulefactory.h"
#include "dspsupermodule.h"
#include "cro_systemobserverfetchtask.h"

namespace DSPSUPERMODULE
{

TaskTemplatePtr DspSuperModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* DspSuperModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new DspSuperModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void DspSuperModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString DspSuperModuleFactory::getFactoryName() const
{
    return QString(DspSuperModule::BaseModuleName).toLower();
}

}

