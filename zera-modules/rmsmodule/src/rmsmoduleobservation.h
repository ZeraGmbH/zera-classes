#ifndef RMSMODULEOBSERVATION_H
#define RMSMODULEOBSERVATION_H

#include <moduleactivist.h>
#include <socket.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace RMSMODULE
{
enum rmsmoduleobservationCmds
{
    registernotifier,
    unregisternotifiers
};

class cRmsModule;

class cRmsModuleObservation: public cModuleActivist
{
    Q_OBJECT

public:
    cRmsModuleObservation(cRmsModule* module, cSocket* pcbsocket);
    virtual ~cRmsModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)

signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes

protected:
    cRmsModule* m_pRmsmodule;
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
    Zera::Proxy::ProxyClient* m_pPCBClient;

private slots:
    void pcbConnect();
    void setNotifier();
    void activationDone();
    void resetNotifier();
    void deactivationDone();
};

}
#endif // RMSMODULEOBSERVATION_H
