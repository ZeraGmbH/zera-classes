#ifndef EFFICIENCY1MODULE_H
#define EFFICIENCY1MODULE_H

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


namespace EFFICIENCY1MODULE
{

class cEfficiency1ModuleConfiguration;
class cEfficiency1ModuleMeasProgram;
class cEfficiency1ModuleObservation;

#define BaseModuleName "Efficiency1Module"
#define BaseSCPIModuleName "EF1"

class cEfficiency1Module : public cBaseMeasModule
{
Q_OBJECT

public:
    cEfficiency1Module(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual ~cEfficiency1Module();
    virtual QByteArray getConfiguration() const;

protected:
    cEfficiency1ModuleObservation *m_pEfficiency1ModuleObservation;
    cEfficiency1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

    void efficiency1ModuleReconfigure();

};

}

#endif // EFFICIENCY1MODULE_H
