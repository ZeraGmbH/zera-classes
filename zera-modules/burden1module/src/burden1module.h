#ifndef BURDEN1MODULE_H
#define BURDEN1MODULE_H

#include "burden1modulemeasprogram.h"
#include <basemeasmodule.h>

namespace BURDEN1MODULE
{
#define BaseModuleName "Burden1Module"
#define BaseSCPIModuleName "BD1"

class cBurden1Module : public cBaseMeasModule
{
Q_OBJECT

public:
    cBurden1Module(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;

protected:
    cBurden1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

    void Burden1ModuleReconfigure();
};

}

#endif // BURDEN1MODULE_H
