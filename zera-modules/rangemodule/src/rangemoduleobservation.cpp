#include "rangemoduleobservation.h"
#include "rangemodule.h"
#include <errormessages.h>
#include <reply.h>

namespace RANGEMODULE
{

cRangeModuleObservation::cRangeModuleObservation(cRangeModule* module, Zera::Proxy::cProxy *proxy, cSocket *pcbsocket)
    :m_pRangemodule(module), m_pProxy(proxy), m_pPCBServerSocket(pcbsocket)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" rangemoduleobservation
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_setNotifierState.addTransition(this, &cRangeModuleObservation::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectState);
    m_activationMachine.addState(&m_setNotifierState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbConnectState);

    connect(&m_pcbConnectState, &QState::entered, this, &cRangeModuleObservation::pcbConnect);
    connect(&m_setNotifierState, &QState::entered, this, &cRangeModuleObservation::setNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cRangeModuleObservation::activationDone);

    m_resetNotifierState.addTransition(this, &cRangeModuleObservation::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState((&m_resetNotifierState));

    connect(&m_resetNotifierState, &QState::entered, this, &cRangeModuleObservation::resetNotifier);
    connect(&m_deactivationDoneState, &QState::entered, this, &cRangeModuleObservation::deactivationDone);
}


cRangeModuleObservation::~cRangeModuleObservation()
{
    delete m_pPCBInterface;
}


void cRangeModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cRangeModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if (msgnr == 0) { // 0 was reserved for async. messages
        QString sintnr;
        // qDebug() << "meas program interrupt";
        sintnr = answer.toString().section(':', 1, 1);
        bool ok;
        int service = sintnr.toInt(&ok);
        switch (service) {
        case 1:
            // we got a sense:mmode notifier
            // let's look what to do
            emit moduleReconfigure();
            break;
        }
    }
    else  {
        int cmd = m_MsgNrCmdList.take(msgnr);
        switch (cmd) {
        case registernotifier:
            if (reply == ack) { // we only continue pcb server acknowledges
                emit activationContinue();
            }
            else {
                emit errMsg((tr(registerpcbnotifierErrMsg)));
#ifdef DEBUG
                qDebug() << registerpcbnotifierErrMsg;
#endif
                emit activationError();
            }
            break;
        case unregisternotifiers:
            if (reply == ack) { // we only continue pcb server acknowledges
                emit deactivationContinue();
            }
            else {
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


void cRangeModuleObservation::pcbConnect()
{
    m_pPCBClient = m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, &Zera::Proxy::cProxyClient::connected, &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cRangeModuleObservation::catchInterfaceAnswer);
    m_pProxy->startConnection(m_pPCBClient);
}


void cRangeModuleObservation::setNotifier()
{
    //m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
    emit activationContinue();
}


void cRangeModuleObservation::activationDone()
{
    emit activated();
}


void cRangeModuleObservation::resetNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisternotifiers;
}


void cRangeModuleObservation::deactivationDone()
{
    m_pProxy->releaseConnection(m_pPCBClient);
    disconnect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cRangeModuleObservation::catchInterfaceAnswer);
    emit deactivated();
}

}
