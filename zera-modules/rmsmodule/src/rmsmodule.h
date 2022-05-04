#ifndef RMSMODULE_H
#define RMSMODULE_H

#include "rmsmoduleobservation.h"
#include "rmsmodulemeasprogram.h"
#include <basemeasmodule.h>
#include <QFinalState>

namespace RMSMODULE
{
#define BaseModuleName "RMSModule"
#define BaseSCPIModuleName "RMS"

class cRmsModule : public cBaseMeasModule
{
Q_OBJECT

public:
    cRmsModule(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;

protected:
    cRmsModuleObservation *m_pRmsModuleObservation;
    cRmsModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

    void rmsModuleReconfigure();

};

}

#endif // RMSMODULE_H
