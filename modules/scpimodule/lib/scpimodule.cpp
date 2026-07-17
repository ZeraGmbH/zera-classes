#include "scpimodule.h"
#include "scpimoduleconfigdata.h"
#include "scpieventsystem.h"

namespace SCPIMODULE
{

cSCPIModule::cSCPIModule(const ModuleFactoryParam &moduleParam) :
    BaseModule(moduleParam),
    m_pSCPIEventSystem(new SCPIEventSystem(this)),
    m_configuration(moduleParam.m_configXmlData),
    m_pModuleValidator(new VfEventSytemModuleParam(moduleParam.m_entityId, moduleParam.m_moduleSharedData->m_storagesystem)),
    m_scpiInterface(getConfigData()->m_sDeviceName),
    m_scpiModelMeasurement(this),
    m_scpiModelParameters(this),
    m_scpiModelCatalogs(this),
    m_scpiModelRpcs(this),
    m_scpiModelDevIface(this)
{
    m_sModuleName = QString("%1%2").arg(BaseModuleName).arg(moduleParam.m_moduleNum);
    m_sModuleDescription = QString("This module provides a scpi interface depending on the actual session running");
    m_sSCPIModuleName = QString("%1%2").arg(BaseSCPIModuleName).arg(moduleParam.m_moduleNum);
    m_cmdEventHandlerSystem = VfCmdEventHandlerSystem::create();
    m_moduleInterfaceParser.parseVeinStorage(moduleParam.m_moduleSharedData->m_storagesystem->getDb());

    m_scpiModelMeasurement.setupScpi(&m_scpiInterface);
    m_scpiModelParameters.setupScpi(&m_scpiInterface);
    m_scpiModelCatalogs.setupScpi(&m_scpiInterface);
    m_scpiModelRpcs.setupScpi(&m_scpiInterface);
    m_scpiModelDevIface.setupScpi(&m_scpiInterface);
    m_scpiModelStatus.setupScpi(&m_scpiInterface);
    m_scpiModelIeee488.setupScpi(&m_scpiInterface);
}

cSCPIModule::~cSCPIModule()
{
    delete m_pModuleValidator;
    emit removeEventSubSystem(m_cmdEventHandlerSystem.get());
}

cSCPIInterface *cSCPIModule::getScpiInterface()
{
    return &m_scpiInterface;
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

void cSCPIModule::updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue)
{
    const QList<cSCPIClient*> &clients = m_pSCPIServer->getClients();
    for (cSCPIClient* client : clients) {
        ScpiStoreScpiSequence* store = client->getMeasureSequenceStore();
        store->updatePendingMeasureSequences(componentId, newValue);
    }
}

ScpiModelCatalogs *cSCPIModule::getScpiModelCatalogs()
{
    return &m_scpiModelCatalogs;
}

VfEventSytemModuleParam *cSCPIModule::getValidatorEventSystem()
{
    return m_pModuleValidator;
}

void cSCPIModule::emitSigSendEvent(QEvent *event)
{
    emit m_pSCPIEventSystem->sigSendEvent(event);
}

VfCmdEventHandlerSystemPtr cSCPIModule::getCmdEventHandlerSystem()
{
    return m_cmdEventHandlerSystem;
}

const VeinScpiModuleInterfaceParser &cSCPIModule::getScpiModuleInterfaceParser() const
{
    return m_moduleInterfaceParser;
}

SignalConnectionDelegateUpdater *cSCPIModule::getSignalDelegateUpdater()
{
    return &m_signalDelegateUpdater;
}

ScpiRpcTransactionStore *cSCPIModule::getRpcTransactionStore()
{
    return &m_rpcTransactionStore;
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
