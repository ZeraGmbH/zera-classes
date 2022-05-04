#include "samplemoduleobservation.h"
#include "samplemodule.h"
#include <errormessages.h>
#include <reply.h>

namespace SAMPLEMODULE
{

cSampleModuleObservation::cSampleModuleObservation(cSampleModule* module, Zera::Proxy::cProxy *proxy, cSocket *pcbsocket)
    :m_pSamplemodule(module), m_pProxy(proxy), m_pPCBServerSocket(pcbsocket)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" samplemoduleobservation
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_setNotifierState.addTransition(this, &cSampleModuleObservation::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectState);
    m_activationMachine.addState(&m_setNotifierState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbConnectState);

    connect(&m_pcbConnectState, &QState::entered, this, &cSampleModuleObservation::pcbConnect);
    connect(&m_setNotifierState, &QState::entered, this, &cSampleModuleObservation::setNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cSampleModuleObservation::activationDone);

    m_resetNotifierState.addTransition(this, &cSampleModuleObservation::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState((&m_resetNotifierState));

    connect(&m_resetNotifierState, &QState::entered, this, &cSampleModuleObservation::resetNotifier);
    connect(&m_deactivationDoneState, &QState::entered, this, &cSampleModuleObservation::deactivationDone);
}


cSampleModuleObservation::~cSampleModuleObservation()
{
    m_pProxy->releaseConnection(m_pPCBClient);
    delete m_pPCBInterface;
}


void cSampleModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cSampleModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
        // qDebug() << "meas program interrupt";
        sintnr = answer.toString().section(':', 1, 1);
        int service = sintnr.toInt(&ok);
        switch (service)
        {
        case 1:
            // we got a sense:mmode notifier
            // let's look what to do
            emit moduleReconfigure();
            break;
        }
    }
    else
    {
        int cmd = m_MsgNrCmdList.take(msgnr);
        switch (cmd)
        {
        case registernotifier:
            if (reply == ack) // we only continue pcb server acknowledges
                emit activationContinue();
            else
            {
                emit errMsg((tr(registerpcbnotifierErrMsg)));
#ifdef DEBUG
                qDebug() << registerpcbnotifierErrMsg;
#endif
                emit activationError();
            }
            break;
        case unregisternotifiers:
            if (reply == ack) // we only continue pcb server acknowledges
                emit deactivationContinue();
            else
            {
                emit errMsg((tr(unregisterpcbnotifierErrMsg)));
#ifdef DEBUG
                qDebug() << unregisterpcbnotifierErrMsg;
#endif
                emit activationError();
            }
            break;
        }
    }
}


void cSampleModuleObservation::pcbConnect()
{
    m_pPCBClient = m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, &Zera::Proxy::cProxyClient::connected, &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cSampleModuleObservation::catchInterfaceAnswer);
    m_pProxy->startConnection(m_pPCBClient);
}


void cSampleModuleObservation::setNotifier()
{
    //m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
    emit activationContinue();
}


void cSampleModuleObservation::activationDone()
{
    emit activated();
}


void cSampleModuleObservation::resetNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisternotifiers;
}


void cSampleModuleObservation::deactivationDone()
{
    emit deactivated();
}

}
