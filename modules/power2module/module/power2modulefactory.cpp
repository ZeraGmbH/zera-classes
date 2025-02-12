#include "power2modulefactory.h"
#include "power2module.h"
#include "cro_systemobserverfetchtask.h"

namespace POWER2MODULE
{

TaskTemplatePtr Power2ModuleFactory::getModulePrepareTask(std::shared_ptr<ModuleSharedData> moduleSharedData)
{
    return ChannelRangeObserver::SystemObserverFetchTask::create(moduleSharedData->m_channelRangeObserver);
}

ZeraModules::VirtualModule* Power2ModuleFactory::createModule(ModuleFactoryParam moduleParam)
{
    return new cPower2Module(moduleParam.getAdjustedParam(m_moduleGroupNumerator.get()));
}

void Power2ModuleFactory::destroyModule(ZeraModules::VirtualModule *module)
{
    m_moduleGroupNumerator->freeModuleNum(module->getModuleNr());
    module->startDestroy();
}

QString Power2ModuleFactory::getFactoryName() const
{
    return QString(cPower2Module::BaseModuleName).toLower();
}

}

