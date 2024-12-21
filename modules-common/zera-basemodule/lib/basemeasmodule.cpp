#include "basemeasmodule.h"

cBaseMeasModule::cBaseMeasModule(ModuleFactoryParam moduleParam, std::shared_ptr<BaseModuleConfiguration> modcfg) :
    BaseModule(moduleParam, modcfg)
{
    m_pModuleValidator = new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_storagesystem);
    m_pModuleEventSystem = m_pModuleValidator;
}

void cBaseMeasModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);
    // now we still have to export the json interface information
    exportMetaData();
    emit activationReady();
}
