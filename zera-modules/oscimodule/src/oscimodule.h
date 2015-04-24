#ifndef OSCIMODULE_H
#define OSCIMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include "basemodule.h"

class cModuleError;

namespace Zera {
namespace Server {
 class cDSPInterface;
}

namespace Proxy {
 class cProxyClient;
}
}


namespace OSCIMODULE
{

class cOsciModuleConfiguration;
class cOsciModuleMeasProgram;
class cOsciModuleObservation;

#define BaseModuleName "OSCModule"
#define BaseSCPIModuleName "OSC"

class cOsciModule : public cBaseModule
{
Q_OBJECT

public:
    cOsciModule(quint8 modnr, Zera::Proxy::cProxy* proxi, VeinPeer* peer, QObject* parent = 0);
    virtual ~cOsciModule();
    virtual QByteArray getConfiguration();

protected:
    cOsciModuleObservation *m_pOsciModuleObservation;
    cOsciModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void unsetModule(); // in case of reconfiguration we must unset module first
    virtual void startMeas(); // we make the measuring program start here
    virtual void stopMeas();

    // our states for base modules activation statemacine
    QState m_ActivationStartState;
    QState m_ActivationExecState;
    QState m_ActivationDoneState;
    QFinalState m_ActivationFinishedState;

    // our states for base modules deactivation statemacine
    QState m_DeactivationStartState;
    QState m_DeactivationExecState;
    QState m_DeactivationDoneState;
    QFinalState m_DeactivationFinishedState;

private:
    qint32 m_nActivationIt;

private slots:
    void activationStart();
    void activationExec();
    void activationDone();
    void activationFinished();

    void deactivationStart();
    void deactivationExec();
    void deactivationDone();
    void deactivationFinished();

    void osciModuleReconfigure();

};

}

#endif // OSCIMODULE_H
