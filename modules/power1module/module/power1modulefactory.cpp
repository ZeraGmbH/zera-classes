#include "power1modulefactory.h"
#include "power1module.h"
#include "cro_systemobserverfetchtask.h"

namespace POWER1MODULE
{

TaskTemplatePtr Power1ModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* Power1ModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cPower1Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Power1ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Power1ModuleFactory::getFactoryName() const
{
    return QString(cPower1Module::BaseModuleName).toLower();
}

}

