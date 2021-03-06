#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QString>
#include <QFile>
#include <virtualmodule.h>
#include <QSet>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <ve_commandevent.h>
#include <vcmp_entitydata.h>
#include <vcmp_componentdata.h>
#include <ve_eventsystem.h>

#include "basemodule.h"
#include "debug.h"
#include "basemoduleconfiguration.h"
#include "veinmodulemetadata.h"
#include "veinmoduleactvalue.h"
#include "veinmodulecomponent.h"
#include "veinmoduleerrorcomponent.h"
#include "veinmoduleparameter.h"
#include "scpiinfo.h"


cBaseModule::cBaseModule(quint8 modnr, Zera::Proxy::cProxy *proxy, int entityId, VeinEvent::StorageSystem *storagesystem, std::shared_ptr<cBaseModuleConfiguration> modcfg, QObject* parent)
    :ZeraModules::VirtualModule(parent), m_pProxy(proxy), m_nEntityId(entityId), m_pStorageSystem(storagesystem), m_pConfiguration(modcfg), m_nModuleNr(modnr)
{
    QString s;
    setParent(parent);

    m_bConfCmd = m_bStartCmd = m_bStopCmd = m_bStateMachineStarted = false;

    m_nStatus = untouched;
    m_pStateMachine = new QStateMachine(this);

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

    m_pStateIdle->addTransition(this, SIGNAL(sigRun()), m_pStateRun); // after sigRun we leave idle
    m_pStateIdle->addTransition(this, SIGNAL(sigStop()), m_pStateStop); // same for sigStop
    m_pStateIDLEIdle = new QState(m_pStateIdle); // the initial state for idle
    m_pStateIDLEConfXML = new QState(m_pStateIdle); // when we configure within idle
    m_pStateIDLEConfSetup = new QState(m_pStateIdle); // same
    m_pStateIDLEIdle->addTransition(this, SIGNAL(sigConfiguration()), m_pStateIDLEConfXML);
    m_pStateIDLEConfXML->addTransition(m_pConfiguration.get(), SIGNAL(configXMLDone()), m_pStateIDLEConfSetup);
    m_pStateIDLEConfSetup->addTransition(this, SIGNAL(sigConfDone()), m_pStateIDLEIdle);
    m_pStateIdle->setInitialState(m_pStateIDLEIdle);
    connect(m_pStateIDLEIdle, SIGNAL(entered()), SLOT(entryIDLEIdle()));
    connect(m_pStateIdle, SIGNAL(entered()), SLOT(entryIdle()));
    connect(m_pStateIdle, SIGNAL(exited()), SLOT(exitIdle()));
    connect(m_pStateIDLEConfXML, SIGNAL(entered()), SLOT(entryConfXML()));
    connect(m_pStateIDLEConfSetup, SIGNAL(entered()), SLOT(entryConfSetup()));

    // we set up our CONFIGURE state here
    // we have nothing to do here because we have some additional states
    // for configuration in idle as well as run and stop because we want to stay in these states

    // we set up our RUN state here

    m_pStateRun->addTransition(this, SIGNAL(sigRunFailed()), m_pStateIdle); // in case of error we fall back to idle
    m_pStateRUNStart = new QState(m_pStateRun);
    m_pStateRUNDone = new QState(m_pStateRun);
    m_pStateRUNDeactivate = new QState(m_pStateRun);
    m_pStateRUNUnset = new QState(m_pStateRun);
    m_pStateRUNConfXML = new QState(m_pStateRun);
    m_pStateRUNConfSetup = new QState(m_pStateRun);
    m_pStateRUNStart->addTransition(this, SIGNAL(activationReady()), m_pStateRUNDone);
    m_pStateRUNDone->addTransition(this, SIGNAL(sigConfiguration()), m_pStateRUNDeactivate);
    m_pStateRUNDeactivate->addTransition(this, SIGNAL(deactivationReady()), m_pStateRUNUnset);
    m_pStateRUNUnset->addTransition(this, SIGNAL(sigReconfigureContinue()), m_pStateRUNConfXML);
    m_pStateRUNConfXML->addTransition(m_pConfiguration.get(), SIGNAL(configXMLDone()), m_pStateRUNConfSetup);
    m_pStateRUNConfSetup->addTransition(this, SIGNAL(sigConfDone()), m_pStateRUNStart );
    m_pStateRun->setInitialState(m_pStateRUNStart);
    connect(m_pStateRUNStart, SIGNAL(entered()), SLOT(entryRunStart()));
    connect(m_pStateRUNDone, SIGNAL(entered()), SLOT(entryRunDone()));
    connect(m_pStateRUNDeactivate, SIGNAL(entered()), SLOT(entryRunDeactivate()));
    connect(m_pStateRUNUnset, SIGNAL(entered()), SLOT(entryRunUnset()));
    connect(m_pStateRUNConfXML, SIGNAL(entered()), SLOT(entryConfXML()));
    connect(m_pStateRUNConfSetup, SIGNAL(entered()), SLOT(entryConfSetup()));


    // we set up our STOP state here

    m_pStateStop->addTransition(this, SIGNAL(sigStopFailed()), m_pStateIdle); // in case of error we fall back to idle
    m_pStateSTOPStart = new QState(m_pStateStop);
    m_pStateSTOPDone = new QState(m_pStateStop);
    m_pStateSTOPDeactivate = new QState(m_pStateStop);
    m_pStateSTOPUnset = new QState(m_pStateStop);
    m_pStateSTOPConfXML = new QState(m_pStateStop);
    m_pStateSTOPConfSetup = new QState(m_pStateStop);
    m_pStateSTOPStart->addTransition(this, SIGNAL(activationReady()), m_pStateSTOPDone);
    m_pStateSTOPDone->addTransition(this, SIGNAL(sigConfiguration()), m_pStateSTOPDeactivate);
    m_pStateSTOPDeactivate->addTransition(this, SIGNAL(deactivationReady()), m_pStateSTOPUnset);
    m_pStateSTOPUnset->addTransition(this, SIGNAL(sigReconfigureContinue()), m_pStateSTOPConfXML);
    m_pStateSTOPConfXML->addTransition(m_pConfiguration.get(), SIGNAL(configXMLDone()), m_pStateSTOPConfSetup);
    m_pStateSTOPConfSetup->addTransition(this, SIGNAL(sigConfDone()), m_pStateSTOPStart );
    m_pStateStop->setInitialState(m_pStateSTOPStart);
    connect(m_pStateSTOPStart, SIGNAL(entered()), SLOT(entryStopStart()));
    connect(m_pStateSTOPDone, SIGNAL(entered()), SLOT(entryStopDone()));
    connect(m_pStateSTOPDeactivate, SIGNAL(entered()), SLOT(entryRunDeactivate())); // we use same slot as run
    connect(m_pStateSTOPUnset, SIGNAL(entered()), SLOT(entryRunUnset()));  // we use same slot as run
    connect(m_pStateSTOPConfXML, SIGNAL(entered()), SLOT(entryConfXML()));
    connect(m_pStateSTOPConfSetup, SIGNAL(entered()), SLOT(entryConfSetup()));

    m_pStateMachine->addState(m_pStateIdle);
    m_pStateMachine->addState(m_pStateConfigure);
    m_pStateMachine->addState(m_pStateRun);
    m_pStateMachine->addState(m_pStateStop);
    m_pStateMachine->addState(m_pStateFinished);
    m_pStateMachine->setInitialState(m_pStateIdle);

    m_pStateMachine->start();

    // now we set up our machines for activating, deactivating a module
    m_ActivationStartState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationExecState);
    m_ActivationExecState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationDoneState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationNext()), &m_ActivationExecState);
    m_ActivationDoneState.addTransition(this, SIGNAL(activationContinue()), &m_ActivationFinishedState);
    m_ActivationMachine.addState(&m_ActivationStartState);
    m_ActivationMachine.addState(&m_ActivationExecState);
    m_ActivationMachine.addState(&m_ActivationDoneState);
    m_ActivationMachine.addState(&m_ActivationFinishedState);
    m_ActivationMachine.setInitialState(&m_ActivationStartState);
    connect(&m_ActivationStartState, SIGNAL(entered()), SLOT(activationStart()));
    connect(&m_ActivationExecState, SIGNAL(entered()), SLOT(activationExec()));
    connect(&m_ActivationDoneState, SIGNAL(entered()), SLOT(activationDone()));
    connect(&m_ActivationFinishedState, SIGNAL(entered()), SLOT(activationFinished()));

    m_DeactivationStartState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationExecState);
    m_DeactivationExecState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationDoneState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationNext()), &m_DeactivationExecState);
    m_DeactivationDoneState.addTransition(this, SIGNAL(deactivationContinue()), &m_DeactivationFinishedState);
    m_DeactivationMachine.addState(&m_DeactivationStartState);
    m_DeactivationMachine.addState(&m_DeactivationExecState);
    m_DeactivationMachine.addState(&m_DeactivationDoneState);
    m_DeactivationMachine.addState(&m_DeactivationFinishedState);
    m_DeactivationMachine.setInitialState(&m_DeactivationStartState);
    connect(&m_DeactivationStartState, SIGNAL(entered()), SLOT(deactivationStart()));
    connect(&m_DeactivationExecState, SIGNAL(entered()), SLOT(deactivationExec()));
    connect(&m_DeactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
    connect(&m_DeactivationFinishedState, SIGNAL(entered()), SLOT(deactivationFinished()));
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

    delete m_pStateMachine;
}


QList<const QState *> cBaseModule::getActualStates() const
{
    return m_StateList;
}


void cBaseModule::setConfiguration(QByteArray xmlConfigData)
{
    m_xmlconfString = xmlConfigData;
    if (m_bStateMachineStarted)
        emit sigConfiguration(); // if our statemachine is already started we emit signal at once
    else
        m_bConfCmd = true; // otherwise we keep in mind that we should configure when machine starts
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
    if (m_bStateMachineStarted)
    {
        emit sigRun(); // if our statemachine is already started we emit signal at once
        startMeas();
    }
    else
        m_bStartCmd = true; // otherwise we keep in mind that we should configure when machine starts
}


void cBaseModule::stopModule()
{
    if (m_bStateMachineStarted)
    {
        emit sigStop(); // if our statemachine is already started we emit signal at once
        stopMeas();
    }
    else
        m_bStopCmd = true; // otherwise we keep in mind that we should configure when machine starts

}


void cBaseModule::setupModule()
{
    VeinComponent::EntityData *eData = new VeinComponent::EntityData();
    eData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    eData->setEntityId(m_nEntityId);
    VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
    m_pModuleEventSystem->sigSendEvent(tmpEvent);

    m_pModuleEntityName = new cVeinModuleComponent(m_nEntityId, m_pModuleEventSystem,
                                                   QString("EntityName"),
                                                   QString("Component forwards the modules name"),
                                                   QVariant(m_sModuleName));

    veinModuleComponentList.append(m_pModuleEntityName);

    m_pModuleErrorComponent = new cVeinModuleErrorComponent(m_nEntityId, m_pModuleEventSystem,
                                                            QString("Error_Messages"),
                                                            m_sModuleName);

    m_pModuleInterfaceComponent = new cVeinModuleComponent(m_nEntityId, m_pModuleEventSystem,
                                                           QString("INF_ModuleInterface"),
                                                           QString("Component forwards the modules interface"),
                                                           QByteArray());

    veinModuleComponentList.append(m_pModuleInterfaceComponent);

    m_pModuleName = new cVeinModuleMetaData(QString("Name"), QVariant(m_sModuleName));
    veinModuleMetaDataList.append(m_pModuleName);

    m_pModuleDescription = new cVeinModuleMetaData(QString("Description"), QVariant(m_sModuleDescription));
    veinModuleMetaDataList.append(m_pModuleDescription);
}


void cBaseModule::unsetModule()
{
    delete m_pModuleErrorComponent;

    if (veinModuleMetaDataList.count() > 0)
    {
        for (int i = 0; i < veinModuleMetaDataList.count(); i++)
            delete veinModuleMetaDataList.at(i);
        veinModuleMetaDataList.clear();
    }

    if (veinModuleComponentList.count() > 0)
    {
        for (int i = 0; i < veinModuleComponentList.count(); i++)
            delete veinModuleComponentList.at(i);
        veinModuleComponentList.clear();
    }

    if (veinModuleActvalueList.count() > 0)
    {
        for (int i = 0; i < veinModuleActvalueList.count(); i++)
            delete veinModuleActvalueList.at(i);
        veinModuleActvalueList.clear();
    }

    if (scpiCommandList.count() > 0)
    {
        for (int i = 0; i < scpiCommandList.count(); i++)
            delete scpiCommandList.at(i);
        scpiCommandList.clear();
    }

    QList<QString> keylist;
    keylist = veinModuleParameterHash.keys();
    if (keylist.count() > 0)
    {
        for (int i = 0; i < keylist.count(); i++)
            delete veinModuleParameterHash[keylist.at(i)];
        veinModuleParameterHash.clear();
    }

    VeinComponent::EntityData *eData = new VeinComponent::EntityData();
    eData->setCommand(VeinComponent::EntityData::Command::ECMD_REMOVE);
    eData->setEntityId(m_nEntityId);
    VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
    m_pModuleEventSystem->sigSendEvent(tmpEvent);

    if (m_ModuleActivistList.count() > 0)
    {
        for (int i = 0; i < m_ModuleActivistList.count(); i++)
        {
            m_ModuleActivistList.at(i)->deleteInterface();
            delete m_ModuleActivistList.at(i);
        }
        m_ModuleActivistList.clear();
    }
}


void cBaseModule::exportMetaData()
{
    QJsonObject jsonObj;
    QJsonObject jsonObj2;

    for (int i = 0; i < veinModuleMetaDataList.count(); i++)
        veinModuleMetaDataList.at(i)->exportMetaData(jsonObj2);

    jsonObj.insert("ModuleInfo", jsonObj2);

    QJsonObject jsonObj3;

    for (int i = 0; i < veinModuleComponentList.count(); i++)
        veinModuleComponentList.at(i)->exportMetaData(jsonObj3);

    for (int i = 0; i < veinModuleActvalueList.count(); i++)
        veinModuleActvalueList.at(i)->exportMetaData(jsonObj3);

    QList<QString> keyList;
    keyList = veinModuleParameterHash.keys();

    for (int i = 0; i < keyList.count(); i++)
        veinModuleParameterHash[keyList.at(i)]->exportMetaData(jsonObj3);

    jsonObj.insert("ComponentInfo", jsonObj3);

    QJsonArray jsonArr;

    // and then all the command information for actual values, parameters and for add. commands without components
    for (int i = 0; i < scpiCommandList.count(); i++)
        scpiCommandList.at(i)->appendSCPIInfo(jsonArr);

    for (int i = 0; i < veinModuleActvalueList.count(); i++)
        veinModuleActvalueList.at(i)->exportSCPIInfo(jsonArr);

    for (int i = 0; i < keyList.count(); i++)
        veinModuleParameterHash[keyList.at(i)]->exportSCPIInfo(jsonArr);


    QJsonObject jsonObj4;

    jsonObj4.insert("Name", m_sSCPIModuleName);
    jsonObj4.insert("Cmd", jsonArr);

    jsonObj.insert("SCPIInfo", jsonObj4);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);

    QByteArray ba;
    ba = jsonDoc.toJson();

#ifdef DEBUG
    qDebug() << jsonDoc.toJson();
#endif

    m_pModuleInterfaceComponent->setValue(QVariant(ba));

}


quint16 cBaseModule::getModuleNr()
{
    return m_nModuleNr;
}


QString cBaseModule::getReleaseNr(QString path)
{
    bool releaseNrFound = false;
    QString releaseNr = "";
    QFile file(path);

    if (file.exists())
    {
       int start, end;
       QString line;

       file.open(QIODevice::ReadOnly);
       QTextStream stream(&file);

       do
       {
           line = stream.readLine();
           if ( (start = line.indexOf("'release")+1) > 0 || (start = line.indexOf("'snapshot")+1) > 0)
           {
                end = line.indexOf("'", start);
                if ((releaseNrFound = (end > start)) == true)
                    releaseNr = line.mid(start, end-start);
           }
       } while (!line.isNull() && !(releaseNrFound));
     }

    file.close();

    return releaseNr;
}

QString cBaseModule::getSerialNr(QString path)
{
    // This is an alternate solution to StatusModule/PAR_SerialNr because
    // statusmodule serves writing of serial number either. Optional writing
    // done by zera-setup2 and in case of success a file
    // /opt/zera/conf/serialnumber (see SerialNoInfoFilePath) is created
    QString serialNo = "";

    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        serialNo = stream.readLine();
        file.close();
    }
    return serialNo;
}


void cBaseModule::entryIdle()
{
    m_StateList.clear(); // we remove all states from list
    m_StateList.append(m_pStateIdle); // but we are in idle now
    m_nLastState = IDLE; // we keep track over our last state
}


void cBaseModule::entryIDLEIdle()
{
    m_bStateMachineStarted = true; // event loop has activated our statemachine
    if (m_bConfCmd)
    {
        m_bConfCmd = false;
        emit sigConfiguration();
    }
    else
        if (m_bStartCmd)
        {
            m_bStartCmd = false;
            emit sigRun();
        }

        else
            if (m_bStopCmd)
            {
                m_bStopCmd = false;
                emit sigStop();
            }
}


void cBaseModule::exitIdle()
{
    // we don't remove state from state list:
    // in case of entering configuration we are in IDLE, CONF
    // in case of entering run or stop statelist is cleared first
}


void cBaseModule::entryConfXML()
{
    m_StateList.append(m_pStateConfigure);
    m_nStatus = untouched; // after each conf. we behave as untouched
    doConfiguration(m_xmlconfString);
}


void cBaseModule::entryConfSetup()
{
    if (m_pConfiguration->isConfigured())
    {
        m_nStatus  = configured;
        setupModule();
        m_nStatus = setup;
    }

    m_StateList.removeOne(m_pStateConfigure);
    emit sigConfDone();
}


void cBaseModule::entryRunStart()
{
    if (m_nStatus == setup) // we must be set up (configured and interface represents configuration)
        m_ActivationMachine.start();
    else
        emit sigRunFailed(); // otherwise we are not able to run
}


void cBaseModule::entryRunDone()
{
    startMeas();
    m_nStatus = activated;
    m_StateList.clear(); // we remove all states
    m_StateList.append(m_pStateRun); // and add run now
    m_nLastState = RUN; // we need this in case of reconfiguration
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
    if (m_nStatus == setup) // we must be set up (configured and interface represents configuration)
        m_ActivationMachine.start();
    else
        emit sigStopFailed(); // otherwise we are not able to run
}


void cBaseModule::entryStopDone()
{
    stopMeas();
    m_nStatus = activated;
    m_StateList.clear(); // we remove all states
    m_StateList.append(m_pStateStop); // and add run now
    m_nLastState = STOP; // we need this in case of reconfiguration
    emit moduleActivated();
}

