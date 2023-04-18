#ifndef THDNMODULE_H
#define THDNMODULE_H

#include "basemeasmodule.h"
#include "dspinterface.h"
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

namespace THDNMODULE {

class cThdnModuleConfiguration;
class cThdnModuleMeasProgram;
class cThdnModuleObservation;

#define BaseModuleName "THDNModule"
#define BaseSCPIModuleName "THD"

class cThdnModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    cThdnModule(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = nullptr);
    virtual QByteArray getConfiguration() const;

protected:
    cThdnModuleObservation *m_pThdnModuleObservation;
    cThdnModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

    void thdnModuleReconfigure();

};

}

#endif // THDNMODULE_H
