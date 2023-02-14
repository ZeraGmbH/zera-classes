#ifndef THDNMODULEOBSERVATION_H
#define THDNMODULEOBSERVATION_H

#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QHash>

#include "moduleactivist.h"
#include "socket.h"
#include <proxyclient.h>

namespace Zera {
namespace  Server {
    class cRMInterface;
    class cPCBInterface;
}
}

namespace THDNMODULE
{
enum thdnmoduleobservationCmds
{
    registernotifier,
    unregisternotifiers
};

class cThdnModule;

class cThdnModuleObservation: public cModuleActivist
{
    Q_OBJECT

public:
    cThdnModuleObservation(cThdnModule* module, cSocket* pcbsocket);
    virtual ~cThdnModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)

signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes

protected:
    cThdnModule* m_pThdnmodule;
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
    Zera::ProxyClient* m_pPCBClient;

private slots:
    void pcbConnect();
    void setNotifier();
    void activationDone();
    void resetNotifier();
    void deactivationDone();
};

}
#endif // THDNMODULEOBSERVATION_H
