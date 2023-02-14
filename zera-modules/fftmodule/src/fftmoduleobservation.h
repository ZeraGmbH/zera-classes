#ifndef FFTMODULEOBSERVATION_H
#define FFTMODULEOBSERVATION_H

#include <moduleactivist.h>
#include <socket.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace FFTMODULE
{
enum fftmoduleobservationCmds
{
    registernotifier,
    unregisternotifiers
};

class cFftModule;

class cFftModuleObservation: public cModuleActivist
{
    Q_OBJECT
public:
    cFftModuleObservation(cFftModule* module, cSocket* pcbsocket);
    virtual ~cFftModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)
signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes
protected:
    cFftModule* m_pFftmodule;
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
#endif // FFTMODULEOBSERVATION_H
