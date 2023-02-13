#include "referencemoduleobservation.h"
#include "referencemodule.h"
#include <errormessages.h>
#include <reply.h>
#include <proxy.h>

namespace REFERENCEMODULE
{

cReferenceModuleObservation::cReferenceModuleObservation(cReferenceModule* module, cSocket *pcbsocket) :
    m_pReferencemodule(module),
    m_pPCBServerSocket(pcbsocket)
{
    m_pPCBInterface = new Zera::Server::cPCBInterface();

    // setting up statemachine for "activating" rferencemoduleobservation
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


cReferenceModuleObservation::~cReferenceModuleObservation()
{
    delete m_pPCBInterface;
}


void cReferenceModuleObservation::generateInterface()
{
    // we don't any interface for this module
}


void cReferenceModuleObservation::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
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


void cReferenceModuleObservation::pcbConnect()
{
    m_pPCBClient = Zera::Proxy::cProxy::getInstance()->getConnection(m_pPCBServerSocket->m_sIP, m_pPCBServerSocket->m_nPort);
    m_pcbConnectState.addTransition(m_pPCBClient, SIGNAL(connected()), &m_setNotifierState);

    m_pPCBInterface->setClient(m_pPCBClient);
    connect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    Zera::Proxy::cProxy::getInstance()->startConnection(m_pPCBClient);
}


void cReferenceModuleObservation::setNotifier()
{
    //m_MsgNrCmdList[m_pPCBInterface->registerNotifier("SENSE:MMODE?","1")] = registernotifier;
    emit activationContinue();
}


void cReferenceModuleObservation::activationDone()
{
    emit activated();
}


void cReferenceModuleObservation::resetNotifier()
{
    m_MsgNrCmdList[m_pPCBInterface->unregisterNotifiers()] = unregisternotifiers;
}


void cReferenceModuleObservation::deactivationDone()
{
    Zera::Proxy::cProxy::getInstance()->releaseConnection(m_pPCBClient);
    disconnect(m_pPCBInterface, SIGNAL(serverAnswer(quint32, quint8, QVariant)), this, SLOT(catchInterfaceAnswer(quint32, quint8, QVariant)));
    emit deactivated();
}

}
