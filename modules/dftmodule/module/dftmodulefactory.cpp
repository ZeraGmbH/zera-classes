#include "dftmodulefactory.h"
#include "dftmodule.h"
#include "cro_systemobserverfetchtask.h"

namespace DFTMODULE
{

TaskTemplatePtr DftModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* DftModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cDftModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void DftModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString DftModuleFactory::getFactoryName() const
{
    return QString(cDftModule::BaseModuleName).toLower();
}

}

