#ifndef SEC1MODULE_H
#define SEC1MODULE_H

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

namespace SEC1MODULE
{


class cSec1ModuleMeasProgram;

#define BaseModuleName "SEC1Module"
#define BaseSCPIModuleName "EC01"

// shortcut of scpi module name is only first 4 characters
// so we will provide ecalculator type EC01 .. EC99
// and each ec will have an additional scpi parent with its number 0001 .. 9999


class cSec1Module : public cBaseMeasModule
{
Q_OBJECT

public:
    cSec1Module(quint8 modnr, Zera::Proxy::cProxy* proxi, int entityId, VeinEvent::StorageSystem* storagesystem, QObject* parent = 0);
    virtual QByteArray getConfiguration() const;

protected:
    cSec1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse

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

    void sec1ModuleReconfigure();

};

}

#endif // SEC1MODULE_H
