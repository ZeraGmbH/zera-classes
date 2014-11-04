#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "power1moduleobservation.h"
#include "power1module.h"

namespace POWER1MODULE
{

cPower1ModuleObservation::cPower1ModuleObservation(cPower1Module* module, Zera::Proxy::cProxy *proxy, cSocket *pcbsocket)
    :m_pPower1module(module), m_pProxy(proxy), m_pPCBServerSocket(pcbsocket)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" rangemoduleobservation
    // m_pcbConnectionState.addTransition is done in pcbConnection
    m_setNotifierState.addTransition(this, SIGNAL(activationContinue()), &m_activationDoneState);

    m_activationMachine.addState(&m_pcbConnectState);
    m_activationMachine.addState(&m_setNotifierState);
    m_activationMachine.addState(&m_activationDoneState);
    m_activationMachine.setInitialState(&m_pcbConnectState);

    connect(&m_pcbConnectState, SIGNAL(entered()), SLOT(pcbConnect()));
    connect(&m_setNotifierState, SIGNAL(entered()), SLOT(setNotifier()));
    connect(&m_activationDoneState, SIGNAL(entered()), SLOT(activationDone()));

    m_resetNotifierState.addTransition(this, SIGNAL(deactivationContinue()), &m_deactivationDoneState);
    m_deactivationMachine.addState(&m_resetNotifierState);
    m_deactivationMachine.addState(&m_deactivationDoneState);
    m_deactivationMachine.setInitialState((&m_resetNotifierState));

    connect(&m_resetNotifierState, SIGNAL(entered()), SLOT(resetNotifier()));
    connect(&m_deactivationDoneState, SIGNAL(entered()), SLOT(deactivationDone()));
}


cPower1ModuleObservation::~cPower1ModuleObservation()
{
    m_pProxy->releaseConnection(m_pPCBClient);
    delete m_pPCBInterface;
}


void cPower1ModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cPower1ModuleObservation::deleteInterface()
{
    // so we have nothing to delete
}


void cPower1ModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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
                emit deactivationError();
            }
            break;
        }
    }
}


void cPower1ModuleObservation::pcbConnect()
{
    Zera::Proxy::cProxyClient* m_pPCBClient = m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pPCBClient);
}


void cPower1ModuleObservation::setNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
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
    emit deactivated();
}

}
