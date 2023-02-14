#include "fftmoduleobservation.h"
#include "fftmodule.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>

namespace FFTMODULE
{

cFftModuleObservation::cFftModuleObservation(cFftModule* module, cSocket *pcbsocket) :
    m_pFftmodule(module),
    m_pPCBServerSocket(pcbsocket)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" rangemoduleobservation
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_setNotifierState.addTransition(this, &cFftModuleObservation::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectState);
    m_activationMachine.addState(&m_setNotifierState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbConnectState);

    connect(&m_pcbConnectState, &QState::entered, this, &cFftModuleObservation::pcbConnect);
    connect(&m_setNotifierState, &QState::entered, this, &cFftModuleObservation::setNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cFftModuleObservation::activationDone);

    m_resetNotifierState.addTransition(this, &cFftModuleObservation::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState((&m_resetNotifierState));

    connect(&m_resetNotifierState, &QState::entered, this, &cFftModuleObservation::resetNotifier);
    connect(&m_deactivationDoneState, &QState::entered, this, &cFftModuleObservation::deactivationDone);
}


cFftModuleObservation::~cFftModuleObservation()
{
    delete m_pPCBInterface;
}


void cFftModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cFftModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    bool ok;

    if (msgnr == 0) // 0 was reserved for async. messages
    {
        QString sintnr;
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
                emit activationError();
            }
            break;
        case unregisternotifiers:
            if (reply == ack) // we only continue pcb server acknowledges
                emit deactivationContinue();
            else
            {
                emit errMsg((tr(unregisterpcbnotifierErrMsg)));
                emit activationError();
            }
            break;
        }
    }
}


void cFftModuleObservation::pcbConnect()
{
    m_pPCBClient = Zera::Proxy::Proxy::getInstance()->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, &Zera::Proxy::ProxyClient::connected, &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cFftModuleObservation::catchInterfaceAnswer);
    Zera::Proxy::Proxy::getInstance()->startConnection(m_pPCBClient);
}


void cFftModuleObservation::setNotifier()
{
    //m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
    emit activationContinue();
}


void cFftModuleObservation::activationDone()
{
    emit activated();
}


void cFftModuleObservation::resetNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisternotifiers;
}


void cFftModuleObservation::deactivationDone()
{
    Zera::Proxy::Proxy::getInstance()->releaseConnection(m_pPCBClient);
    disconnect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cFftModuleObservation::catchInterfaceAnswer);
    emit deactivated();
}

}
