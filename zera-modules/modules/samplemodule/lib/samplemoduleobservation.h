#ifndef SAMPLEMODULEOBSERVATION_H
#define SAMPLEMODULEOBSERVATION_H

#include <moduleactivist.h>
#include <socket.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace SAMPLEMODULE
{
enum samplemoduleobservationCmds
{
    registernotifier,
    unregisternotifiers
};

class cSampleModule;

class cSampleModuleObservation: public cModuleActivist
{
    Q_OBJECT
public:
    cSampleModuleObservation(cSampleModule* module, cSocket* pcbsocket);
    virtual ~cSampleModuleObservation();
    virtual void generateInterface(); // here we export our interface (entities)
signals:
    void moduleReconfigure(); // we emit a signal for module reconfiguration when we recognize significant changes
protected:
    cSampleModule* m_pSamplemodule;
    cSocket* m_pPCBServerSocket;
    Zera::cPCBInterface* m_pPCBInterface;
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

#endif // SAMPLEMODULEOBSERVATION_H
