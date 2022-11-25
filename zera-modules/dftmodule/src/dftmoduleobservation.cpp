#include "dftmoduleobservation.h"
#include "dftmodule.h"
#include <errormessages.h>
#include <reply.h>
#include <pcbinterface.h>

namespace DFTMODULE
{

cDftModuleObservation::cDftModuleObservation(cDftModule* module, Zera::Proxy::cProxy *proxy, cSocket *pcbsocket)
    :m_pDftmodule(module), m_pProxy(proxy), m_pPCBServerSocket(pcbsocket)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" rangemoduleobservation
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_setNotifierState.addTransition(this, &cDftModuleObservation::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectState);
    m_activationMachine.addState(&m_setNotifierState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbConnectState);

    connect(&m_pcbConnectState, &QState::entered, this, &cDftModuleObservation::pcbConnect);
    connect(&m_setNotifierState, &QState::entered, this, &cDftModuleObservation::setNotifier);
    connect(&m_activationDoneState, &QState::entered, this, &cDftModuleObservation::activationDone);

    m_resetNotifierState.addTransition(this, &cDftModuleObservation::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState((&m_resetNotifierState));

    connect(&m_resetNotifierState, &QState::entered, this, &cDftModuleObservation::resetNotifier);
    connect(&m_deactivationDoneState, &QState::entered, this, &cDftModuleObservation::deactivationDone);
}


cDftModuleObservation::~cDftModuleObservation()
{
    delete m_pPCBInterface;
}


void cDftModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cDftModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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


void cDftModuleObservation::pcbConnect()
{
    m_pPCBClient = m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, &Zera::Proxy::cProxyClient::connected, &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cDftModuleObservation::catchInterfaceAnswer);
    m_pProxy->startConnection(m_pPCBClient);
}


void cDftModuleObservation::setNotifier()
{
    //m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
    emit activationContinue();
}


void cDftModuleObservation::activationDone()
{
    emit activated();
}


void cDftModuleObservation::resetNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisternotifiers;
}


void cDftModuleObservation::deactivationDone()
{
    m_pProxy->releaseConnection(m_pPCBClient);
    disconnect(m_pPCBInterface, &Zera::Server::cPCBInterface::serverAnswer, this, &cDftModuleObservation::catchInterfaceAnswer);
    emit deactivated();
}

}
