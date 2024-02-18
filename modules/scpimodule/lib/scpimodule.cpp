#include "scpimodule.h"
#include "scpimoduleconfiguration.h"
#include "scpimoduleconfigdata.h"
#include "scpiserver.h"
#include "vfmoduleerrorcomponent.h"
#include "scpieventsystem.h"
#include <vfmodulemetadata.h>
#include <vfmoduleactvalue.h>
#include <vfmoduleparameter.h>
#include <scpiinfo.h>
#include <ve_storagesystem.h>
#include <ve_commandevent.h>
#include <vcmp_entitydata.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


namespace SCPIMODULE
{

cSCPIModule::cSCPIModule(MeasurementModuleFactoryParam moduleParam) :
    cBaseModule(moduleParam, std::shared_ptr<cBaseModuleConfiguration>(new cSCPIModuleConfiguration()))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a scpi interface depending on the actual session running");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);

    m_pSCPIEventSystem = new cSCPIEventSystem(this);
    m_pModuleValidator = new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_storagesystem);
}


QByteArray cSCPIModule::getConfiguration() const
{
    return m_pConfiguration->exportConfiguration();
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
    cBaseModule::setupModule();

    cSCPIModuleConfigData *pConfData;
    pConfData = qobject_cast<cSCPIModuleConfiguration*>(m_pConfiguration.get())->getConfigurationData();

    // we only have this activist
    m_pSCPIServer = new cSCPIServer(this, *pConfData);
    m_ModuleActivistList.append(m_pSCPIServer);
    connect(m_pSCPIServer, &cSCPIServer::activated, this, &cSCPIModule::activationContinue);
    connect(m_pSCPIServer, &cSCPIServer::deactivated, this, &cSCPIModule::deactivationContinue);
    connect(m_pSCPIServer, &cSCPIServer::errMsg, m_pModuleErrorComponent, &VfModuleErrorComponent::setValue);

    // we already post meta information here because setting up interface looks for valid meta info
    exportMetaData();

    for (int i = 0; i < m_ModuleActivistList.count(); i++)
        m_ModuleActivistList.at(i)->generateInterface();
}


void cSCPIModule::startMeas()
{
    // we don't start the interface expl.
}


void cSCPIModule::stopMeas()
{
    // we don't stop the interface expl.
}


void cSCPIModule::activationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit activationContinue();
}


void cSCPIModule::activationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->activate();
}


void cSCPIModule::activationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit activationNext(); // and iterate over our list
    else
        emit activationContinue();
}


void cSCPIModule::activationFinished()
{
    m_pModuleValidator->setParameterMap(m_veinModuleParameterMap);

    // we post meta information once again because it's complete now
    exportMetaData();
    emit addEventSystem(m_pSCPIEventSystem);
    emit activationReady();
}


void cSCPIModule::deactivationStart()
{
    m_nActivationIt = 0; // we start with the first
    emit deactivationContinue();
}


void cSCPIModule::deactivationExec()
{
    m_ModuleActivistList.at(m_nActivationIt)->deactivate();
}


void cSCPIModule::deactivationDone()
{
    m_nActivationIt++;

    if (m_nActivationIt < m_ModuleActivistList.count())
        emit deactivationNext(); // and iterate over our list
    else
        emit deactivationContinue();
}


void cSCPIModule::deactivationFinished()
{
    emit deactivationReady();
}

}
