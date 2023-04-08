#include "power1moduleobservation.h"
#include "power1module.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>

namespace POWER1MODULE
{

cPower1ModuleObservation::cPower1ModuleObservation(cPower1Module* module, cSocket *pcbsocket) :
    m_pPower1module(module),
    m_pPCBServerSocket(pcbsocket)
{
    m_pPCBInterface = new Zera::cPCBInterface();

    // setting up statemachine for "activating" rangemoduleobservation
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_setNotifierState.addTransition(this, &cPower1ModuleObservation::activationContinue, &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectState);
    m_activationMachine.addState(&m_setNotifierState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbConnectState);

    connect(&m_pcbConnectState, &QAbstractState::entered, this, &cPower1ModuleObservation::pcbConnect);
    connect(&m_setNotifierState, &QAbstractState::entered, this, &cPower1ModuleObservation::setNotifier);
    connect(&m_activationDoneState, &QAbstractState::entered, this, &cPower1ModuleObservation::activationDone);

    m_resetNotifierState.addTransition(this, &cPower1ModuleObservation::deactivationContinue, &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState((&m_resetNotifierState));

    connect(&m_resetNotifierState, &QAbstractState::entered, this, &cPower1ModuleObservation::resetNotifier);
    connect(&m_deactivationDoneState, &QAbstractState::entered, this, &cPower1ModuleObservation::deactivationDone);
}


cPower1ModuleObservation::~cPower1ModuleObservation()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient);
    delete m_pPCBInterface;
}


void cPower1ModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cPower1ModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
                emit deactivationError();
            }
            break;
        }
    }
}


void cPower1ModuleObservation::pcbConnect()
{
    m_pPCBClient = Zera::Proxy::getInstance()->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, &Zera::ProxyClient::connected, &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, &AbstractServerInterface::serverAnswer, this, &cPower1ModuleObservation::catchInterfaceAnswer);
    Zera::Proxy::getInstance()->startConnection(m_pPCBClient);
}


void cPower1ModuleObservation::setNotifier()
{
    //m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
    emit activationContinue();
}


void cPower1ModuleObservation::activationDone()
{
    emit activated();
}


void cPower1ModuleObservation::resetNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisternotifiers;
}


void cPower1ModuleObservation::deactivationDone()
{
    Zera::Proxy::getInstance()->releaseConnection(m_pPCBClient);
    disconnect(m_pPCBInterface, &AbstractServerInterface::serverAnswer, this, &cPower1ModuleObservation::catchInterfaceAnswer);
    emit deactivated();
}

}
