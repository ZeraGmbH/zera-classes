#include "apimodule.h"
#include "apimoduleauthorize.h"
#include "apimoduleconfiguration.h"
#include <errormessages.h>

namespace APIMODULE
{

VfEventSytemModuleParam *cApiModule::getValidatorEventSystem()
{
    return m_pModuleValidator;
}

cApiModule::~cApiModule()
{
    delete m_rpcEventSystem;
}

VfModuleRpc *cApiModule::getRpcEventSystem() const
{
    return m_rpcEventSystem;
}

cApiModule::cApiModule(ModuleFactoryParam moduleParam)
    : BaseModule(moduleParam, std::make_shared<cApiModuleConfiguration>()),
    m_rpcEventSystem(new VfModuleRpc(moduleParam.m_entityId)),
    m_pModuleValidator(new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_moduleSharedData->m_storagesystem))
{
    m_sModuleName = QString(BaseModuleName);
    m_sModuleDescription = QString("This module supports API access");
    m_sSCPIModuleName = QString(BaseSCPIModuleName);
    m_pModuleEventSystem = new VfEventSytemModuleParam(moduleParam.m_entityId,
                                                       moduleParam.m_moduleSharedData->m_storagesystem);
}

void cApiModule::activationFinished()
{
    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);

    // we post meta information once again because it's complete now
    exportMetaData();
    emit activationReady();
}

void cApiModule::setupModule()
{
    emit addEventSystem(getValidatorEventSystem());
    emit addEventSystem(m_rpcEventSystem);
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
