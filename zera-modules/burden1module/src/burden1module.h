#ifndef BURDEN1MODULE_H
#define BURDEN1MODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include "basemeasmodule.h"

class cModuleError;

namespace Zera {
namespace Server {
 class cDSPInterface;
}

namespace Proxy {
 class cProxyClient;
}
}


namespace BURDEN1MODULE
{

class cBurden1ModuleConfiguration;
class cBurden1ModuleMeasProgram;
class cBurden1ModuleObservation;

#define BaseModuleName "Burden1Module"
#define BaseSCPIModuleName "BD1"

class cBurden1Module : public cBaseMeasModule
{
Q_OBJECT

public:
    cBurden1Module(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;

protected:
    cBurden1ModuleObservation *m_pBurden1ModuleObservation;
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
