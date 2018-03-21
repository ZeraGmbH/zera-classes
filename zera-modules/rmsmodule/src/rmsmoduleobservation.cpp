#include <pcbinterface.h>
#include <proxy.h>
#include <proxyclient.h>

#include "debug.h"
#include "errormessages.h"
#include "reply.h"
#include "rmsmoduleobservation.h"
#include "rmsmodule.h"

namespace RMSMODULE
{

cRmsModuleObservation::cRmsModuleObservation(cRmsModule* module, Zera::Proxy::cProxy *proxy, cSocket *pcbsocket)
    :m_pRmsmodule(module), m_pProxy(proxy), m_pPCBServerSocket(pcbsocket)
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


cRmsModuleObservation::~cRmsModuleObservation()
{
    delete m_pPCBInterface;
}


void cRmsModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cRmsModuleObservation::deleteInterface()
{
    // so we have nothing to delete
}


void cRmsModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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


void cRmsModuleObservation::pcbConnect()
{
    m_pPCBClient = m_pProxy->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    m_pProxy->startConnection(m_pPCBClient);
}


void cRmsModuleObservation::setNotifier()
{
    //m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
    emit activationContinue();
}


void cRmsModuleObservation::activationDone()
{
    emit activated();
}


void cRmsModuleObservation::resetNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisternotifiers;
}


void cRmsModuleObservation::deactivationDone()
{
    m_pProxy->releaseConnection(m_pPCBClient);
    disconnect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    emit deactivated();
}

}
