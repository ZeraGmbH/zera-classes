#include "apimodule.h"
#include "apimoduleauthorize.h"
#include "apimoduleconfiguration.h"
#include <errormessages.h>

namespace APIMODULE
{

cApiModule::cApiModule(ModuleFactoryParam moduleParam) : BaseModule(moduleParam, std::make_shared<cApiModuleConfiguration>()),
    m_pModuleValidator(new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_moduleSharedData->m_storagesystem))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module supports API access");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
    m_pModuleEventSystem = new VfEventSytemModuleParam(moduleParam.m_entityId,
                                                       moduleParam.m_moduleSharedData->m_storagesystem);
}

void cApiModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // we post meta information once again because it's complete now
    exportMetaData();
    emit activationReady();
}

void cApiModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    emit addEventSystem(m_pModuleEventSystem);

    cApiModuleAuthorize* moduleActivist = new cApiModuleAuthorize(this);
    m_ModuleActivistList.append(moduleActivist);
    BaseModule::setupModule();

    moduleActivist->generateVeinInterface();

    emit activationContinue();

}

void cApiModule::startMeas()
{
}

void cApiModule::stopMeas()
{
}

}
