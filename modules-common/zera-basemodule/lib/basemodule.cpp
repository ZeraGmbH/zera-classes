#include "basemodule.h"
#include "basemoduleconfiguration.h"
#include "vfmodulemetadata.h"
#include "vfmoduleactvalue.h"
#include "vfmodulecomponent.h"
#include "vfmoduleerrorcomponent.h"
#include "vfmoduleparameter.h"
#include "vf_server_entity_add.h"
#include "vf_server_entity_remove.h"
#include "scpiinfo.h"
#include <virtualmodule.h>
#include <QJsonDocument>
#include <QFile>

cBaseModule::cBaseModule(ModuleFactoryParam moduleParam, std::shared_ptr<cBaseModuleConfiguration> modcfg) :
    ZeraModules::VirtualModule(moduleParam.m_moduleNum, moduleParam.m_entityId),
    m_pConfiguration(modcfg),
    m_moduleParam(moduleParam)
{
    QString s;

    m_bStartCmd = m_bStopCmd = m_bStateMachineStarted = false;

    m_nStatus = untouched;
    m_xmlconfString = moduleParam.m_configXmlData;
    m_pConfiguration->setConfiguration(m_xmlconfString);
    m_bConfCmd = m_pConfiguration->isConfigured();

    // our states from virtualmodule (interface)
    m_pStateIdle = new QState();
    m_pStateIdle->setObjectName(s = "IDLE");
    m_pStateConfigure = new QState();
    m_pStateConfigure->setObjectName(s = "CONFIGURE");
    m_pStateRun = new QState();
    m_pStateRun->setObjectName(s = "RUN");
    m_pStateStop = new QState();
    m_pStateStop->setObjectName(s = "STOP");
    m_pStateFinished = new QFinalState();
    m_pStateFinished->setObjectName(s = "FINISH");

    // we set up our IDLE state here

    m_pStateIdle->addTransition(this, &cBaseModule::sigRun, m_pStateRun); // after sigRun we leave idle
    m_pStateIdle->addTransition(this, &cBaseModule::sigStop, m_pStateStop); // same for sigStop
    m_pStateIDLEIdle = new QState(m_pStateIdle); // the initial state for idle
    m_pStateIDLEConfSetup = new QState(m_pStateIdle); // same
    m_pStateIDLEIdle->addTransition(this, &cBaseModule::sigConfiguration, m_pStateIDLEConfSetup);
    m_pStateIDLEConfSetup->addTransition(this, &cBaseModule::sigConfDone, m_pStateIDLEIdle);
    m_pStateIdle->setInitialState(m_pStateIDLEIdle);
    connect(m_pStateIDLEIdle, &QState::entered, this, &cBaseModule::entryIDLEIdle);
    connect(m_pStateIdle, &QState::entered, this, &cBaseModule::entryIdle);
    connect(m_pStateIDLEConfSetup, &QState::entered, this, &cBaseModule::entryConfSetup);

    // we set up our CONFIGURE state here
    // we have nothing to do here because we have some additional states
    // for configuration in idle as well as run and stop because we want to stay in these states

    // we set up our RUN state here

    m_pStateRun->addTransition(this, &cBaseModule::sigRunFailed, m_pStateIdle); // in case of error we fall back to idle
    m_pStateRUNStart = new QState(m_pStateRun);
    m_pStateRUNDone = new QState(m_pStateRun);
    m_pStateRUNDeactivate = new QState(m_pStateRun);
    m_pStateRUNUnset = new QState(m_pStateRun);
    m_pStateRUNConfSetup = new QState(m_pStateRun);
    m_pStateRUNStart->addTransition(this, &cBaseModule::activationReady, m_pStateRUNDone);
    m_pStateRUNDone->addTransition(this, &cBaseModule::sigConfiguration, m_pStateRUNDeactivate);
    m_pStateRUNDeactivate->addTransition(this, &cBaseModule::deactivationReady, m_pStateRUNUnset);
    m_pStateRUNUnset->addTransition(this, &cBaseModule::sigReconfigureContinue, m_pStateRUNConfSetup);
    m_pStateRUNConfSetup->addTransition(this, &cBaseModule::sigConfDone, m_pStateRUNStart );
    m_pStateRun->setInitialState(m_pStateRUNStart);
    connect(m_pStateRUNStart, &QState::entered, this, &cBaseModule::entryRunStart);
    connect(m_pStateRUNDone, &QState::entered, this, &cBaseModule::entryRunDone);
    connect(m_pStateRUNDeactivate, &QState::entered, this, &cBaseModule::entryRunDeactivate);
    connect(m_pStateRUNUnset, &QState::entered, this, &cBaseModule::entryRunUnset);
    connect(m_pStateRUNConfSetup, &QState::entered, this, &cBaseModule::entryConfSetup);

    // we set up our STOP state here

    m_pStateStop->addTransition(this, &cBaseModule::sigStopFailed, m_pStateIdle); // in case of error we fall back to idle
    m_pStateSTOPStart = new QState(m_pStateStop);
    m_pStateSTOPDone = new QState(m_pStateStop);
    m_pStateSTOPDeactivate = new QState(m_pStateStop);
    m_pStateSTOPUnset = new QState(m_pStateStop);
    m_pStateSTOPConfSetup = new QState(m_pStateStop);
    m_pStateSTOPStart->addTransition(this, &cBaseModule::activationReady, m_pStateSTOPDone);
    m_pStateSTOPDone->addTransition(this, &cBaseModule::sigConfiguration, m_pStateSTOPDeactivate);
    m_pStateSTOPDeactivate->addTransition(this, &cBaseModule::deactivationReady, m_pStateSTOPUnset);
    m_pStateSTOPUnset->addTransition(this, &cBaseModule::sigReconfigureContinue, m_pStateSTOPConfSetup);
    m_pStateSTOPConfSetup->addTransition(this, &cBaseModule::sigConfDone, m_pStateSTOPStart );
    m_pStateStop->setInitialState(m_pStateSTOPStart);
    connect(m_pStateSTOPStart, &QState::entered, this, &cBaseModule::entryStopStart);
    connect(m_pStateSTOPDone, &QState::entered, this, &cBaseModule::entryStopDone);
    connect(m_pStateSTOPDeactivate, &QState::entered, this, &cBaseModule::entryRunDeactivate); // we use same slot as run
    connect(m_pStateSTOPUnset, &QState::entered, this, &cBaseModule::entryRunUnset);  // we use same slot as run
    connect(m_pStateSTOPConfSetup, &QState::entered, this, &cBaseModule::entryConfSetup);

    m_stateMachine.addState(m_pStateIdle);
    m_stateMachine.addState(m_pStateConfigure);
    m_stateMachine.addState(m_pStateRun);
    m_stateMachine.addState(m_pStateStop);
    m_stateMachine.addState(m_pStateFinished);
    m_stateMachine.setInitialState(m_pStateIdle);

    m_stateMachine.start();

    // now we set up our machines for activating, deactivating a module
    m_ActivationStartState.addTransition(this, &cBaseModule::activationContinue, &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, &cBaseModule::activationContinue, &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, &cBaseModule::activationNext, &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, &cBaseModule::activationContinue, &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, &QState::entered, this, &cBaseModule::activationStart);
    connect(&m_ActivationExecState, &QState::entered, this, &cBaseModule::activationExec);
    connect(&m_ActivationDoneState, &QState::entered, this, &cBaseModule::activationDone);
    connect(&m_ActivationFinishedState, &QState::entered, this, &cBaseModule::activationFinished);

    m_DeactivationStartState.addTransition(this, &cBaseModule::deactivationContinue, &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, &cBaseModule::deactivationContinue, &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, &cBaseModule::deactivationNext, &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, &cBaseModule::deactivationContinue, &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, &QState::entered, this, &cBaseModule::deactivationStart);
    connect(&m_DeactivationExecState, &QState::entered, this, &cBaseModule::deactivationExec);
    connect(&m_DeactivationDoneState, &QState::entered, this, &cBaseModule::deactivationDone);
    connect(&m_DeactivationFinishedState, &QState::entered, this, &cBaseModule::deactivationFinished);
}


cBaseModule::~cBaseModule()
{
    unsetModule();

    delete m_pStateIDLEIdle;
    delete m_pStateIDLEConfSetup;

    delete m_pStateRUNStart;
    delete m_pStateRUNDone;
    delete m_pStateRUNDeactivate;
    delete m_pStateRUNUnset;
    delete m_pStateRUNConfSetup;

    delete m_pStateSTOPStart;
    delete m_pStateSTOPDone;
    delete m_pStateSTOPDeactivate;
    delete m_pStateSTOPUnset;
    delete m_pStateSTOPConfSetup;

    delete m_pStateIdle;
    delete m_pStateConfigure;
    delete m_pStateRun;
    delete m_pStateStop;
    delete m_pStateFinished;
}

bool cBaseModule::isConfigured() const
{
    return (m_nStatus > untouched);
}

void cBaseModule::startModule()
{
    if (m_bStateMachineStarted) {
        emit sigRun(); // if our statemachine is already started we emit signal at once
        startMeas();
    }
    else {
        m_bStartCmd = true; // otherwise we keep in mind that we should configure when machine starts
    }
}

void cBaseModule::stopModule()
{
    if (m_bStateMachineStarted) {
        emit sigStop(); // if our statemachine is already started we emit signal at once
        stopMeas();
    }
    else {
        m_bStopCmd = true; // otherwise we keep in mind that we should configure when machine starts
    }

}

void cBaseModule::setupModule()
{
    emit m_pModuleEventSystem->sigSendEvent(VfServerEntityAdd::generateEvent(getEntityId()));

    m_pModuleEntityName = new VfModuleComponent(getEntityId(), m_pModuleEventSystem,
                                                   QString("EntityName"),
                                                   QString("Module's name"),
                                                   QVariant(m_sModuleName));

    veinModuleComponentList.append(m_pModuleEntityName);

    m_pModuleErrorComponent = new VfModuleErrorComponent(getEntityId(), m_pModuleEventSystem,
                                                            QString("Error_Messages"),
                                                            m_sModuleName);

    m_pModuleInterfaceComponent = new VfModuleComponent(getEntityId(), m_pModuleEventSystem,
                                                           QString("INF_ModuleInterface"),
                                                           QString("Module's interface details"),
                                                           QByteArray());

    veinModuleComponentList.append(m_pModuleInterfaceComponent);

    m_pModuleName = new VfModuleMetaData(QString("Name"), QVariant(m_sModuleName));
    veinModuleMetaDataList.append(m_pModuleName);

    m_pModuleDescription = new VfModuleMetaData(QString("Description"), QVariant(m_sModuleDescription));
    veinModuleMetaDataList.append(m_pModuleDescription);
}


void cBaseModule::unsetModule()
{
    delete m_pModuleErrorComponent;

    for (auto veinModuleMetaData : qAsConst(veinModuleMetaDataList)) {
        delete veinModuleMetaData;
    }
    veinModuleMetaDataList.clear();

    for (auto veinModuleComponent : qAsConst(veinModuleComponentList)) {
        delete veinModuleComponent;
    }
    veinModuleComponentList.clear();

    for (auto veinModuleActvalue : qAsConst(veinModuleActvalueList)) {
        delete veinModuleActvalue;
    }
    veinModuleActvalueList.clear();

    for (auto scpiCommand : qAsConst(scpiCommandList)) {
        delete scpiCommand;
    }
    scpiCommandList.clear();

    for (auto veinModuleParameter : qAsConst(m_veinModuleParameterMap)) {
        delete veinModuleParameter;
    }
    m_veinModuleParameterMap.clear();

    emit m_pModuleEventSystem->sigSendEvent(VfServerEntityRemove::generateEvent(getEntityId()));

    for (auto ModuleActivist : qAsConst(m_ModuleActivistList)) {
        delete ModuleActivist;
    }
    m_ModuleActivistList.clear();
}

void cBaseModule::exportMetaData()
{
    QJsonObject jsonObj;
    QJsonObject jsonObj2;

    for (int i = 0; i < veinModuleMetaDataList.count(); i++) {
        veinModuleMetaDataList.at(i)->exportMetaData(jsonObj2);
    }
    jsonObj.insert("ModuleInfo", jsonObj2);

    QJsonObject jsonObj3;
    for (int i = 0; i < veinModuleComponentList.count(); i++) {
        veinModuleComponentList.at(i)->exportMetaData(jsonObj3);
    }

    for (int i = 0; i < veinModuleActvalueList.count(); i++) {
        veinModuleActvalueList.at(i)->exportMetaData(jsonObj3);
    }

    QList<QString> keyList;
    keyList = m_veinModuleParameterMap.keys();
    for (int i = 0; i < keyList.count(); i++)
        m_veinModuleParameterMap[keyList.at(i)]->exportMetaData(jsonObj3);
    jsonObj.insert("ComponentInfo", jsonObj3);

    QJsonArray jsonArr;
    // and then all the command information for actual values, parameters and for add. commands without components
    for (int i = 0; i < scpiCommandList.count(); i++) {
        scpiCommandList.at(i)->appendSCPIInfo(jsonArr);
    }
    for (int i = 0; i < veinModuleActvalueList.count(); i++) {
        veinModuleActvalueList.at(i)->exportSCPIInfo(jsonArr);
    }

    for (int i = 0; i < keyList.count(); i++) {
        m_veinModuleParameterMap[keyList.at(i)]->exportSCPIInfo(jsonArr);
    }

    QJsonObject jsonObj4;
    jsonObj4.insert("Name", m_sSCPIModuleName);
    jsonObj4.insert("Cmd", jsonArr);
    jsonObj.insert("SCPIInfo", jsonObj4);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    QByteArray ba;
    ba = jsonDoc.toJson();

    m_pModuleInterfaceComponent->setValue(QVariant(ba));
}

VeinEvent::StorageSystem *cBaseModule::getStorageSystem()
{
    return m_moduleParam.m_storagesystem;
}

bool cBaseModule::getDemo()
{
    return m_moduleParam.m_demo;
}

AbstractFactoryServiceInterfacesPtr cBaseModule::getServiceInterfaceFactory()
{
    return m_moduleParam.m_serviceInterfaceFactory;
}

VeinTcp::AbstractTcpNetworkFactoryPtr cBaseModule::getTcpNetworkFactory()
{
    return m_moduleParam.m_tcpNetworkFactory;
}

void cBaseModule::entryIdle()
{
}

void cBaseModule::entryIDLEIdle()
{
    m_bStateMachineStarted = true; // event loop has activated our statemachine
    if (m_bConfCmd) {
        m_bConfCmd = false;
        emit sigConfiguration();
    }
    else {
        if (m_bStartCmd) {
            m_bStartCmd = false;
            emit sigRun();
        }
        else {
            if (m_bStopCmd)
            {
                m_bStopCmd = false;
                emit sigStop();
            }
        }
    }
}

void cBaseModule::entryConfSetup()
{
    if (m_pConfiguration->isConfigured()) {
        m_nStatus  = configured;
        setupModule();
        m_nStatus = setup;
    }
    emit sigConfDone();
}

void cBaseModule::entryRunStart()
{
    if (m_nStatus == setup) { // we must be set up (configured and interface represents configuration)
        m_ActivationMachine.start();
    }
    else {
        emit sigRunFailed(); // otherwise we are not able to run
    }
}

void cBaseModule::entryRunDone()
{
    startMeas();
    m_nStatus = activated;
    emit moduleActivated();
}

void cBaseModule::entryRunDeactivate()
{
    m_DeactivationMachine.start();
}

void cBaseModule::entryRunUnset()
{
    unsetModule();
    emit sigReconfigureContinue();
}

void cBaseModule::entryStopStart()
{
    if (m_nStatus == setup) { // we must be set up (configured and interface represents configuration)
        m_ActivationMachine.start();
    }
    else {
        emit sigStopFailed(); // otherwise we are not able to run
    }
}

void cBaseModule::entryStopDone()
{
    stopMeas();
    m_nStatus = activated;
    emit moduleActivated();
}
