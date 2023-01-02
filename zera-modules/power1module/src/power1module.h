#ifndef POWER1MODULE_H
#define POWER1MODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>
#include <memory> // std header for smart pointer

#include "basemeasmodule.h"
#include <vfinputcomponentseventsystem.h>

namespace Zera {
namespace Server {
 class cDSPInterface;
}

namespace Proxy {
 class cProxyClient;
}
}


namespace POWER1MODULE
{

class cPower1ModuleConfiguration;
class cPower1ModuleMeasProgram;
class cPower1ModuleObservation;

#define BaseModuleName "POWER1Module"
#define BaseSCPIModuleName "POW"

class cPower1Module : public cBaseMeasModule
{
Q_OBJECT

public:
    cPower1Module(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;

    VfInputComponentsEventSystem *getPEventSystem() const;

protected:
    cPower1ModuleObservation *m_pPower1ModuleObservation;
    cPower1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    // we do not inherit cBaseMeasWorkProgram so have an own event system for input components :(
    // came in 6828db17069aa94f62a976ebd3e15061976d0006 - the start of pre-scale mess
    VfInputComponentsEventSystem *m_inputComponentEventSystem;
    virtual void doConfiguration(QByteArray xmlConfigData); // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
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

    void power1ModuleReconfigure();

};

}

#endif // POWER1MODULE_H
