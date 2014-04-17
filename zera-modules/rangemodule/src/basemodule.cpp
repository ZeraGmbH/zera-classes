#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QString>
#include <virtualmodule.h>

#include "basemodule.h"
#include "basemoduleconfiguration.h"


cBaseModule::cBaseModule(Zera::Proxy::cProxy *proxy, VeinPeer *peer, cBaseModuleConfiguration* modcfg, QObject* parent)
    :ZeraModules::VirtualModule(proxy,peer,parent), m_pProxy(proxy), m_pPeer(peer), m_pConfiguration(modcfg)
{
    QString s;
    setParent(parent);

    m_ConfigTimer.setSingleShot(true);
    m_StartTimer.setSingleShot(true);

    m_nStatus = BaseModule::untouched;
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

    // our additional states
    // for configuration
    m_pStateConfXML = new QState(m_pStateConfigure);
    m_pStateConfSetup = new QState(m_pStateConfigure);
    m_pStateConfigure->setInitialState(m_pStateConfXML);
    m_pStateConfXML->addTransition(m_pConfiguration, SIGNAL(configXMLDone()), m_pStateConfSetup);

    // our additional states
    // for run
    m_pStateRunStart = new QState(m_pStateRun);
    m_pStateRunDone = new QState(m_pStateRun);
    m_pStateRun->setInitialState(m_pStateRunStart);
    m_pStateRunStart->addTransition(this, SIGNAL(sigInitDone()), m_pStateRunDone);

    // our additional states
    // for stop
    m_pStateStopStart = new QState(m_pStateStop);
    m_pStateStopDone = new QState(m_pStateStop);
    m_pStateStop->setInitialState(m_pStateStopStart);
    m_pStateStopStart->addTransition(this, SIGNAL(sigInitDone()), m_pStateStopDone);

    m_pStateMachine->addState(m_pStateIdle);
    m_pStateMachine->addState(m_pStateConfigure);
    m_pStateMachine->addState(m_pStateRun);
    m_pStateMachine->addState(m_pStateStop);
    m_pStateMachine->addState(m_pStateFinished);

    m_pStateMachine->setInitialState(m_pStateIdle);

    m_pStateIdle->addTransition(this, SIGNAL(sigConfiguration()), m_pStateConfigure);
    m_pStateConfigure->addTransition(this, SIGNAL(sigConfDoneIdle()), m_pStateIdle);

    m_pStateIdle->addTransition(this, SIGNAL(sigStop()), m_pStateStop);
    m_pStateStop->addTransition(this, SIGNAL(sigStopFailed()), m_pStateIdle);

    m_pStateIdle->addTransition(this, SIGNAL(sigRun()), m_pStateRun);
    m_pStateRun->addTransition(this, SIGNAL(sigRunFailed()), m_pStateIdle);

    m_pStateRun->addTransition(this, SIGNAL(sigStop()), m_pStateStop);
    m_pStateRun->addTransition(this, SIGNAL(sigRun()), m_pStateRun);
    m_pStateRun->addTransition(this, SIGNAL(sigConfiguration()), m_pStateConfigure);

    m_pStateStop->addTransition(this, SIGNAL(sigRun()), m_pStateRun);
    m_pStateStop->addTransition(this, SIGNAL(sigStop()), m_pStateStop);
    m_pStateStop->addTransition(this, SIGNAL(sigConfiguration()), m_pStateConfigure);

    connect(m_pStateIdle, SIGNAL(entered()), SLOT(entryIdle()));
    connect(m_pStateIdle, SIGNAL(exited()), SLOT(exitIdle()));
    connect(m_pStateConfigure, SIGNAL(entered()), SLOT(entryConf()));
    connect(m_pStateConfigure, SIGNAL(exited()), SLOT(exitConf()));
    connect(m_pStateConfXML, SIGNAL(entered()), SLOT(entryConfXML()));
    connect(m_pStateConfSetup, SIGNAL(entered()), SLOT(entryConfSetup()));
    connect(m_pStateRunStart, SIGNAL(entered()), SLOT(entryRunStart()));
    connect(m_pStateRunDone, SIGNAL(entered()), SLOT(entryRunDone()));
    connect(m_pStateStopStart, SIGNAL(entered()), SLOT(entryStopStart()));
    connect(m_pStateStopDone, SIGNAL(entered()), SLOT(entryStopDone()));
    connect(&m_ConfigTimer, SIGNAL(timeout()), SIGNAL(sigConfiguration()));
    connect(&m_StartTimer, SIGNAL(timeout()), SIGNAL(sigRun()));
    m_pStateMachine->start();
}


cBaseModule::~cBaseModule()
{
    delete m_pStateIdle;
    delete m_pStateConfigure;
    delete m_pStateRun;
    delete m_pStateStop;
    delete m_pStateFinished;

    delete m_pStateConfXML;
    delete m_pStateConfSetup;

    delete m_pStateRunStart;
    delete m_pStateRunDone;

    delete m_pStateStopStart;
    delete m_pStateStopDone;

    delete m_pStateMachine;
}


QList<const QState *> cBaseModule::getActualStates()
{
    return m_StateList;
}


void cBaseModule::setConfiguration(QByteArray xmlConfigData)
{
    m_xmlconfString = xmlConfigData;
    m_ConfigTimer.start(0); // in case eventloop is not yet running
}


bool cBaseModule::isConfigured()
{
    return (m_nStatus > BaseModule::untouched);
}


void cBaseModule::startModule()
{
    m_StartTimer.start(0); // in case eventloop is not yet running
}


void cBaseModule::stopModule()
{
    emit sigStop();
    // here we must inform our module engine
}


void cBaseModule::entryIdle()
{
    m_StateList.clear(); // we remove all states from list
    m_StateList.append(m_pStateIdle); // but we are in idle now
    m_nLastState = BaseModule::IDLE; // we keep track over our last state
}


void cBaseModule::exitIdle()
{
    // we don't remove state from state list:
    // in case of entering configuration we are in IDLE, CONF
    // in case of entering run or stop statelist is cleared first
}


void cBaseModule::entryConf()
{
    m_StateList.append(m_pStateConfigure);
    // keep in mind that we enter m_pStateConfSetup also !!!
}


void cBaseModule::exitConf()
{
    m_StateList.removeOne(m_pStateConfigure);
}


void cBaseModule::entryConfXML()
{
    m_nStatus = BaseModule::untouched; // after each conf. we behave as untouched
    doConfiguration(m_xmlconfString);
}


void cBaseModule::entryConfSetup()
{
    if (m_pConfiguration->isConfigured())
    {
        m_nStatus = BaseModule::configured;
        setupModule(); // we only setup our module after configuration success
        m_nStatus = BaseModule::setup;
    }

    switch (m_nLastState) // and depended on where we came from we exit configuration
    {
    case BaseModule::IDLE:
        emit sigConfDoneIdle();
        break;
    case BaseModule::RUN:
        emit sigConfDoneRun();
        break;
    case BaseModule::STOP:
        emit sigConfDoneStop();
        break;
    }
}


void cBaseModule::entryRunStart()
{
    if (m_nStatus >= BaseModule::setup) // we must be set up at least (configured and interface represents configuration)
    {
        if (m_nStatus == BaseModule::setup)
            doInitialization();
        else
            emit sigInitDone(); // if we are already initialized
    }

    else
        emit sigRunFailed(); // otherwise we are not able to run
}


void cBaseModule::entryRunDone()
{
    startMeas();
    m_nStatus = BaseModule::initialized;
    m_StateList.clear(); // we remove all states
    m_StateList.append(m_pStateRun); // and add run now
    m_nLastState = BaseModule::RUN; // we need this in case of reconfiguration
}


void cBaseModule::entryStopStart()
{
    if (m_nStatus >= BaseModule::setup) // we must be set up at least (configured and interface represents configuration)
    {
        if (m_nStatus == BaseModule::setup)
            doInitialization();
        else
            emit sigInitDone(); // if we are already intialized
    }

    else
        emit sigStopFailed(); // otherwise we are not able to run
}


void cBaseModule::entryStopDone()
{
    stopMeas();
    m_nStatus = BaseModule::initialized;
    m_StateList.clear(); // we remove all states
    m_StateList.append(m_pStateStop); // and add run now

    m_nLastState = BaseModule::STOP; // we need this in case of reconfiguration
}
