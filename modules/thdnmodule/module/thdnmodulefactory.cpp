#include "thdnmodulefactory.h"
#include "thdnmodule.h"
#include "cro_systemobserverfetchtask.h"

namespace THDNMODULE
{

TaskTemplatePtr ThdnModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* ThdnModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cThdnModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void ThdnModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString ThdnModuleFactory::getFactoryName() const
{
    return QString(cThdnModule::BaseModuleName).toLower();
}

}

