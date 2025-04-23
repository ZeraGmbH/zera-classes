#include "basemeasmodule.h"

cBaseMeasModule::cBaseMeasModule(ModuleFactoryParam moduleParam, std::shared_ptr<BaseModuleConfiguration> modcfg) :
    BaseModule(moduleParam, modcfg)
{
    m_pModuleEventSystem = new VfEventSytemModuleParam(moduleParam.m_entityId,
                                                       moduleParam.m_moduleSharedData->m_storagesystem);
}

VfEventSytemModuleParam *cBaseMeasModule::getValidatorEventSystem()
{
    return static_cast<VfEventSytemModuleParam *>(m_pModuleEventSystem);
}

void cBaseMeasModule::activationFinished()
{
    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);
    // now we still have to export the json interface information
    exportMetaData();
    emit activationReady();
}
