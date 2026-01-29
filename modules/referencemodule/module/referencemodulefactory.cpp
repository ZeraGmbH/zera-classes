#include "referencemodulefactory.h"
#include "referencemodule.h"
#include "cro_systemobserverfetchtask.h"

namespace REFERENCEMODULE
{

TaskTemplatePtr ReferenceModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* ReferenceModuleFactory::createModule(const ModuleFactoryParam &moduleParam)
{
    return new cReferenceModule(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void ReferenceModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString ReferenceModuleFactory::getFactoryName() const
{
    return QString(cReferenceModule::BaseModuleName).toLower();
}

}

