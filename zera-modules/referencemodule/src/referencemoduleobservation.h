#ifndef REFERENCEMODULEOBSERVATION_H
#define REFERENCEMODULEOBSERVATION_H

#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QHash>

#include "moduleactivist.h"
#include "socket.h"
#include "reply.h"


namespace Zera {
namespace Proxy {
    class cProxy;
    class cProxyClient;
}
namespace  Server {
    class cRMInterface;
    class cPCBInterface;
}
}


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
    cReferenceModuleObservation(cReferenceModule* module, Zera::Proxy::cProxy* proxy,cSocket* pcbsocket);
    virtual ~cReferenceModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    virtual void exportInterface(QJsonArray &);

signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes

protected:
    cReferenceModule* m_pReferencemodule;
    Zera::Proxy::cProxy* m_pProxy;
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
