#ifndef POWER1MODULEOBSERVATION_H
#define POWER1MODULEOBSERVATION_H

#include <moduleactivist.h>
#include <socket.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace POWER1MODULE
{
enum rmsmoduleobservationCmds
{
    registernotifier,
    unregisternotifiers
};

class cPower1Module;

class cPower1ModuleObservation: public cModuleActivist
{
    Q_OBJECT
public:
    cPower1ModuleObservation(cPower1Module* module, cSocket* pcbsocket);
    virtual ~cPower1ModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)
signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes
protected:
    cPower1Module* m_pPower1module;
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
#endif // POWER1MODULEOBSERVATION_H
