#include "oscimodulefactory.h"
#include "oscimodule.h"
#include "cro_systemobserverfetchtask.h"

namespace OSCIMODULE
{

TaskTemplatePtr OsciModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* OsciModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cOsciModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void OsciModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString OsciModuleFactory::getFactoryName() const
{
    return QString(cOsciModule::BaseModuleName).toLower();
}

}

