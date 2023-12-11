#ifndef POWER1MODULE_H
#define POWER1MODULE_H

#include "basemeasmodule.h"
#include "dspinterface.h"
#include <vfeventsysteminputcomponents.h>
#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>
#include <memory> // std header for smart pointer

namespace POWER1MODULE {

class cPower1ModuleConfiguration;
class cPower1ModuleMeasProgram;

class cPower1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "POWER1Module";
    static constexpr const char* BaseSCPIModuleName = "POW";

    cPower1Module(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, bool demo);
    virtual QByteArray getConfiguration() const;

    VfEventSystemInputComponents *getPEventSystem() const;

protected:
    cPower1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    // we do not inherit cBaseMeasWorkProgram so have an own event system for input components :(
    // came in 6828db17069aa94f62a976ebd3e15061976d0006 - the start of pre-scale mess
    VfEventSystemInputComponents *m_inputComponentEventSystem;
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
};

}

#endif // POWER1MODULE_H
