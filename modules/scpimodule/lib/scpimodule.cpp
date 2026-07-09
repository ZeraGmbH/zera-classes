#include "scpimodule.h"
#include "scpimoduleconfigdata.h"
#include "scpieventsystem.h"

namespace SCPIMODULE
{

cSCPIModule::cSCPIModule(const ModuleFactoryParam &moduleParam) :
    BaseModule(moduleParam),
    m_moduleCommonPendingMeasureStore(std::make_shared<QMultiHash<QString, VeinComponentScpiMeasureSequence*>>()),
    m_pSCPIEventSystem(new SCPIEventSystem(this)),
    m_configuration(moduleParam.m_configXmlData),
    m_pModuleValidator(new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_moduleSharedData->m_storagesystem))
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a scpi interface depending on the actual session running");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
    m_cmdEventHandlerSystem = VfCmdEventHandlerSystem::create();
}

cSCPIModule::~cSCPIModule()
{
    delete m_pModuleValidator;
    emit removeEventSubSystem(m_cmdEventHandlerSystem.get());
}

cSCPIServer *cSCPIModule::getSCPIServer()
{
    return m_pSCPIServer;
}

cSCPIModuleConfigData *cSCPIModule::getConfigData()
{
    return m_configuration.getConfigData();
}

QByteArray cSCPIModule::getConfigXml() const
{
    return m_configuration.exportConfiguration();
}

void cSCPIModule::removeClient(cSCPIClient *client)
{
    for(auto iter = m_scpiVeinParamOrRpcTransactions.begin(); iter != m_scpiVeinParamOrRpcTransactions.end(); ) {
        if(iter.value()->getClient() == client)
            iter = m_scpiVeinParamOrRpcTransactions.erase(iter);
        else
            iter++;
    }
}

void cSCPIModule::insertScpiVeinParamRpcTransaction(const QString &componentOrRpcName,
                                                    const SCPIVeinTransactionInfoPtr &transactionInfo)
{
    m_scpiVeinParamOrRpcTransactions.insert(componentOrRpcName, transactionInfo);
}

void cSCPIModule::removeScpiVeinParamRpcTransaction(const QString &componentOrRpcName,
                                                    const SCPIVeinTransactionInfoPtr &transactionInfo)
{
    m_scpiVeinParamOrRpcTransactions.remove(componentOrRpcName, transactionInfo);
}

const QList<SCPIVeinTransactionInfoPtr> cSCPIModule::getAllScpiVeinParamRpcTransactions()
{
    return m_scpiVeinParamOrRpcTransactions.values();
}

const QList<SCPIVeinTransactionInfoPtr> cSCPIModule::getScpiVeinParamRpcTransactions(const QString &componentOrRpcName)
{
    return m_scpiVeinParamOrRpcTransactions.values(componentOrRpcName);
}

VfEventSytemModuleParam *cSCPIModule::getValidatorEventSystem()
{
    return m_pModuleValidator;
}

VfCmdEventHandlerSystemPtr cSCPIModule::getCmdEventHandlerSystem()
{
    return m_cmdEventHandlerSystem;
}

void cSCPIModule::setupModule()
{
    emit addEventSubSystem(m_cmdEventHandlerSystem.get());

    emit addEventSystem(getValidatorEventSystem());
    emit addEventSystem(m_pSCPIEventSystem);

    m_pModuleEventSystem = m_pSCPIEventSystem;
    BaseModule::setupModule();

    cSCPIModuleConfigData *pConfData = m_configuration.getConfigData();

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
    getValidatorEventSystem()->setParameterMap(m_veinModuleParameterMap);

    // we post meta information once again because it's complete now
    exportMetaData();
    emit addEventSystem(m_pSCPIEventSystem);
    emit activationReady();
}

}
