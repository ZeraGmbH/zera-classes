#include "basemeasmodule.h"

cBaseMeasModule::cBaseMeasModule(ModuleFactoryParam moduleParam, std::shared_ptr<cBaseModuleConfiguration> modcfg) :
    cBaseModule(moduleParam, modcfg)
{
    m_pModuleValidator = new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_storagesystem);
    m_pModuleEventSystem = m_pModuleValidator;
}
