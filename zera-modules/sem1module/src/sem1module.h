#ifndef SEM1MODULE_H
#define SEM1MODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

#include "basemeasmodule.h"


namespace Zera {
namespace Server {
 class cDSPInterface;
}

namespace Proxy {
 class cProxyClient;
}
}

class cModuleInfo;
class cModuleError;

namespace SEM1MODULE
{


class cSem1ModuleMeasProgram;

#define BaseModuleName "SEM1Module"
#define BaseSCPIModuleName "EM01"

// shortcut of scpi module name is only first 4 characters
// so we will provide energy measurement type EM01 .. EM99
// and each energy measurement  will have an additional scpi parent with its number 0001 .. 9999


class cSem1Module : public cBaseMeasModule
{
Q_OBJECT

public:
    cSem1Module(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual ~cSem1Module();
    virtual QByteArray getConfiguration() const;

protected:
    cSem1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse

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

    void sem1ModuleReconfigure();

};

}

#endif // SEM1MODULE_H
