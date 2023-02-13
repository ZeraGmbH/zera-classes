#ifndef REFERENCEMODULEOBSERVATION_H
#define REFERENCEMODULEOBSERVATION_H

#include <moduleactivist.h>
#include <socket.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace REFERENCEMODULE
{
enum referencemoduleobservationCmds
{
    registernotifier,
    unregisternotifiers
};

class cReferenceModule;

class cReferenceModuleObservation: public cModuleActivist
{
    Q_OBJECT
public:
    cReferenceModuleObservation(cReferenceModule* module, cSocket* pcbsocket);
    virtual ~cReferenceModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)
signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes
protected:
    cReferenceModule* m_pReferencemodule;
    cSocket* m_pPCBServerSocket;
    Zera::Server::cPCBInterface* m_pPCBInterface;
protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:   
    // statemachine for activating a referencemoduleobservation
    QState m_pcbConnectState; // we try to get a connection to our pcb server
    QState m_setNotifierState; // we set our notifier here
    QFinalState m_activationDoneState;

    // statemachine for deactivating a referencemoduleobservation
    QState m_resetNotifierState; // we reset our notifier here
    QFinalState m_deactivationDoneState;

    QHash<quint32, int> m_MsgNrCmdList;
    Zera::Proxy::cProxyClient* m_pPCBClient;

private slots:
    void pcbConnect();
    void setNotifier();
    void activationDone();
    void resetNotifier();
    void deactivationDone();
};

}

#endif // REFERENCEMODULEOBSERVATION_H
