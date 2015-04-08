#ifndef OSCIMODULEOBSERVATION_H
#define OSCIMODULEOBSERVATION_H

#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QHash>

#include "moduleactivist.h"
#include "socket.h"

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

namespace OSCIMODULE
{
enum oscimoduleobservationCmds
{
    registernotifier,
    unregisternotifiers
};

class cOsciModule;

class cOsciModuleObservation: public cModuleActivist
{
    Q_OBJECT

public:
    cOsciModuleObservation(cOsciModule* module, Zera::Proxy::cProxy* proxy,cSocket* pcbsocket);
    virtual ~cOsciModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    virtual void exportInterface(QJsonArray &);

signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes

protected:
    cOsciModule* m_pOscimodule;
    Zera::Proxy::cProxy* m_pProxy;
    cSocket* m_pPCBServerSocket;

    Zera::Server::cPCBInterface* m_pPCBInterface;

protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:   
    // statemachine for activating a rangemoduleobservation
    QState m_pcbConnectState; // we try to get a connection to our pcb server
    QState m_setNotifierState; // we set our notifier here
    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemoduleobservation
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
#endif // OSCIMODULEOBSERVATION_H
