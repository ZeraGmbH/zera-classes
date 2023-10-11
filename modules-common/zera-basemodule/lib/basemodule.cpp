#include "basemodule.h"
#include "basemoduleconfiguration.h"
#include "vfmodulemetadata.h"
#include "vfmoduleactvalue.h"
#include "vfmodulecomponent.h"
#include "vfmoduleerrorcomponent.h"
#include "vfmoduleparameter.h"
#include "scpiinfo.h"
#include <ve_commandevent.h>
#include <vcmp_entitydata.h>
#include <vcmp_componentdata.h>
#include <ve_eventsystem.h>
#include <virtualmodule.h>
#include <QJsonDocument>
#include <QFile>

cBaseModule::cBaseModule(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, std::shared_ptr<cBaseModuleConfiguration> modcfg, bool demo, QObject* parent) :
    ZeraModules::VirtualModule(parent),
    m_nEntityId(entityId),
    m_pStorageSystem(storagesystem),
    m_demo(demo),
    m_pConfiguration(modcfg),
    m_nModuleNr(modnr)
{
    QString s;
    setParent(parent);

    m_bConfCmd = m_bStartCmd = m_bStopCmd = m_bStateMachineStarted = false;

    m_nStatus = untouched;

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
    m_pStateIDLEConfXML = new QState(m_pStateIdle); // when we configure within idle
    m_pStateIDLEConfSetup = new QState(m_pStateIdle); // same
    m_pStateIDLEIdle->addTransition(this, &cBaseModule::sigConfiguration, m_pStateIDLEConfXML);
    m_pStateIDLEConfXML->addTransition(m_pConfiguration.get(), &cBaseModuleConfiguration::configXMLDone, m_pStateIDLEConfSetup);
    m_pStateIDLEConfSetup->addTransition(this, &cBaseModule::sigConfDone, m_pStateIDLEIdle);
    m_pStateIdle->setInitialState(m_pStateIDLEIdle);
    connect(m_pStateIDLEIdle, &QState::entered, this, &cBaseModule::entryIDLEIdle);
    connect(m_pStateIdle, &QState::entered, this, &cBaseModule::entryIdle);
    connect(m_pStateIDLEConfXML, &QState::entered, this, &cBaseModule::entryConfXML);
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
    m_pStateRUNConfXML = new QState(m_pStateRun);
    m_pStateRUNConfSetup = new QState(m_pStateRun);
    m_pStateRUNStart->addTransition(this, &cBaseModule::activationReady, m_pStateRUNDone);
    m_pStateRUNDone->addTransition(this, &cBaseModule::sigConfiguration, m_pStateRUNDeactivate);
    m_pStateRUNDeactivate->addTransition(this, &cBaseModule::deactivationReady, m_pStateRUNUnset);
    m_pStateRUNUnset->addTransition(this, &cBaseModule::sigReconfigureContinue, m_pStateRUNConfXML);
    m_pStateRUNConfXML->addTransition(m_pConfiguration.get(), &cBaseModuleConfiguration::configXMLDone, m_pStateRUNConfSetup);
    m_pStateRUNConfSetup->addTransition(this, &cBaseModule::sigConfDone, m_pStateRUNStart );
    m_pStateRun->setInitialState(m_pStateRUNStart);
    connect(m_pStateRUNStart, &QState::entered, this, &cBaseModule::entryRunStart);
    connect(m_pStateRUNDone, &QState::entered, this, &cBaseModule::entryRunDone);
    connect(m_pStateRUNDeactivate, &QState::entered, this, &cBaseModule::entryRunDeactivate);
    connect(m_pStateRUNUnset, &QState::entered, this, &cBaseModule::entryRunUnset);
    connect(m_pStateRUNConfXML, &QState::entered, this, &cBaseModule::entryConfXML);
    connect(m_pStateRUNConfSetup, &QState::entered, this, &cBaseModule::entryConfSetup);

    // we set up our STOP state here

    m_pStateStop->addTransition(this, &cBaseModule::sigStopFailed, m_pStateIdle); // in case of error we fall back to idle
    m_pStateSTOPStart = new QState(m_pStateStop);
    m_pStateSTOPDone = new QState(m_pStateStop);
    m_pStateSTOPDeactivate = new QState(m_pStateStop);
    m_pStateSTOPUnset = new QState(m_pStateStop);
    m_pStateSTOPConfXML = new QState(m_pStateStop);
    m_pStateSTOPConfSetup = new QState(m_pStateStop);
    m_pStateSTOPStart->addTransition(this, &cBaseModule::activationReady, m_pStateSTOPDone);
    m_pStateSTOPDone->addTransition(this, &cBaseModule::sigConfiguration, m_pStateSTOPDeactivate);
    m_pStateSTOPDeactivate->addTransition(this, &cBaseModule::deactivationReady, m_pStateSTOPUnset);
    m_pStateSTOPUnset->addTransition(this, &cBaseModule::sigReconfigureContinue, m_pStateSTOPConfXML);
    m_pStateSTOPConfXML->addTransition(m_pConfiguration.get(), &cBaseModuleConfiguration::configXMLDone, m_pStateSTOPConfSetup);
    m_pStateSTOPConfSetup->addTransition(this, &cBaseModule::sigConfDone, m_pStateSTOPStart );
    m_pStateStop->setInitialState(m_pStateSTOPStart);
    connect(m_pStateSTOPStart, &QState::entered, this, &cBaseModule::entryStopStart);
    connect(m_pStateSTOPDone, &QState::entered, this, &cBaseModule::entryStopDone);
    connect(m_pStateSTOPDeactivate, &QState::entered, this, &cBaseModule::entryRunDeactivate); // we use same slot as run
    connect(m_pStateSTOPUnset, &QState::entered, this, &cBaseModule::entryRunUnset);  // we use same slot as run
    connect(m_pStateSTOPConfXML, &QState::entered, this, &cBaseModule::entryConfXML);
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
    delete m_pStateIDLEConfXML;
    delete m_pStateIDLEConfSetup;

    delete m_pStateRUNStart;
    delete m_pStateRUNDone;
    delete m_pStateRUNDeactivate;
    delete m_pStateRUNUnset;
    delete m_pStateRUNConfXML;
    delete m_pStateRUNConfSetup;

    delete m_pStateSTOPStart;
    delete m_pStateSTOPDone;
    delete m_pStateSTOPDeactivate;
    delete m_pStateSTOPUnset;
    delete m_pStateSTOPConfXML;
    delete m_pStateSTOPConfSetup;

    delete m_pStateIdle;
    delete m_pStateConfigure;
    delete m_pStateRun;
    delete m_pStateStop;
    delete m_pStateFinished;
}

void cBaseModule::setConfiguration(QByteArray xmlConfigData)
{
    m_xmlconfString = xmlConfigData;
    if (m_bStateMachineStarted) {
        emit sigConfiguration(); // if our statemachine is already started we emit signal at once
    }
    else {
        m_bConfCmd = true; // otherwise we keep in mind that we should configure when machine starts
    }
}

QString cBaseModule::getModuleName()
{
    return m_sModuleName;
}

QString cBaseModule::getSCPIModuleName()
{
    return m_sSCPIModuleName;
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
    VeinComponent::EntityData *eData = new VeinComponent::EntityData();
    eData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    eData->setEntityId(m_nEntityId);
    VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
    m_pModuleEventSystem->sigSendEvent(tmpEvent);

    m_pModuleEntityName = new VfModuleComponent(m_nEntityId, m_pModuleEventSystem,
                                                   QString("EntityName"),
                                                   QString("Module's name"),
                                                   QVariant(m_sModuleName));

    veinModuleComponentList.append(m_pModuleEntityName);

    m_pModuleErrorComponent = new VfModuleErrorComponent(m_nEntityId, m_pModuleEventSystem,
                                                            QString("Error_Messages"),
                                                            m_sModuleName);

    m_pModuleInterfaceComponent = new VfModuleComponent(m_nEntityId, m_pModuleEventSystem,
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

    for (auto veinModuleMetaData : veinModuleMetaDataList) {
        delete veinModuleMetaData;
    }
    veinModuleMetaDataList.clear();

    for (auto veinModuleComponent : veinModuleComponentList) {
        delete veinModuleComponent;
    }
    veinModuleComponentList.clear();

    for (auto veinModuleActvalue : veinModuleActvalueList) {
        delete veinModuleActvalue;
    }
    veinModuleActvalueList.clear();

    for (auto scpiCommand : scpiCommandList) {
        delete scpiCommand;
    }
    scpiCommandList.clear();

    for (auto veinModuleParameter : veinModuleParameterHash) {
        delete veinModuleParameter;
    }
    veinModuleParameterHash.clear();

    VeinComponent::EntityData *eData = new VeinComponent::EntityData();
    eData->setCommand(VeinComponent::EntityData::Command::ECMD_REMOVE);
    eData->setEntityId(m_nEntityId);
    VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
    m_pModuleEventSystem->sigSendEvent(tmpEvent);

    for (auto ModuleActivist : m_ModuleActivistList) {
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
    keyList = veinModuleParameterHash.keys();
    for (int i = 0; i < keyList.count(); i++)
        veinModuleParameterHash[keyList.at(i)]->exportMetaData(jsonObj3);
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
        veinModuleParameterHash[keyList.at(i)]->exportSCPIInfo(jsonArr);
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

quint16 cBaseModule::getModuleNr()
{
    return m_nModuleNr;
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

void cBaseModule::entryConfXML()
{
    m_nStatus = untouched; // after each conf. we behave as untouched
    doConfiguration(m_xmlconfString);
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
