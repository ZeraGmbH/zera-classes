#include "scpimodule.h"
#include "scpimoduleconfiguration.h"
#include "scpimoduleconfigdata.h"
#include "scpieventsystem.h"

namespace SCPIMODULE
{

cSCPIModule::cSCPIModule(ModuleFactoryParam moduleParam) :
    BaseModule(moduleParam, std::make_shared<cSCPIModuleConfiguration>()),
    m_pSCPIEventSystem(new SCPIEventSystem(this)),
    m_pModuleValidator(new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_storagesystem))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a scpi interface depending on the actual session running");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
}

cSCPIServer *cSCPIModule::getSCPIServer()
{
    return m_pSCPIServer;
}

void cSCPIModule::setupModule()
{
    emit addEventSystem(m_pModuleValidator);
    emit addEventSystem(m_pSCPIEventSystem);

    m_pModuleEventSystem = m_pSCPIEventSystem;
    BaseModule::setupModule();

    cSCPIModuleConfigData *pConfData = qobject_cast<cSCPIModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we only have this activist
    m_pSCPIServer = new cSCPIServer(this, *pConfData);
    m_ModuleActivistList.append(m_pSCPIServer);
    connect(m_pSCPIServer, &cSCPIServer::activated, this, &cSCPIModule::activationContinue);
    connect(m_pSCPIServer, &cSCPIServer::deactivated, this, &cSCPIModule::deactivationContinue);

    // we already post meta information here because setting up interface looks for valid meta info
    exportMetaData();

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateVeinInterface();
}

void cSCPIModule::startMeas()
{
    // we don't start the interface expl.
}

void cSCPIModule::stopMeas()
{
    // we don't stop the interface expl.
}

void cSCPIModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // we post meta information once again because it's complete now
    exportMetaData();
    emit addEventSystem(m_pSCPIEventSystem);
    emit activationReady();
}

}
