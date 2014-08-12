#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QString>
#include <virtualmodule.h>
#include <QSet>

#include "basemodule.h"
#include "basemoduleconfiguration.h"

namespace RANGEMODULE
{

cBaseModule::cBaseModule(quint8 modnr, Zera::Proxy::cProxy *proxy, VeinPeer *peer, cBaseModuleConfiguration* modcfg, QObject* parent)
    :ZeraModules::VirtualModule(proxy,peer,parent), m_pProxy(proxy), m_pPeer(peer), m_pConfiguration(modcfg), m_nModuleNr(modnr)
{
    QString s;
    setParent(parent);

    m_ConfigTimer.setSingleShot(true);
    m_StartTimer.setSingleShot(true);

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
    m_pStateIDLEConfXML->addTransition(m_pConfiguration, SIGNAL(configXMLDone()), m_pStateIDLEConfSetup);
    m_pStateIDLEConfSetup->addTransition(this, SIGNAL(sigConfDone()), m_pStateIDLEIdle);
    m_pStateIdle->setInitialState(m_pStateIDLEIdle);
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
    m_pStateRUNConfXML->addTransition(m_pConfiguration, SIGNAL(configXMLDone()), m_pStateRUNConfSetup);
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
    m_pStateSTOPConfXML->addTransition(m_pConfiguration, SIGNAL(configXMLDone()), m_pStateSTOPConfSetup);
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
    return (m_nStatus > untouched);
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


quint8 cBaseModule::getModuleNr()
{
    return m_nModuleNr;
}


void cBaseModule::entryIdle()
{
    m_StateList.clear(); // we remove all states from list
    m_StateList.append(m_pStateIdle); // but we are in idle now
    m_nLastState = IDLE; // we keep track over our last state
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
}

}

