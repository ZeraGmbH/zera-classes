#ifndef MODEMODULE_H
#define MODEMODULE_H

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
}

namespace MODEMODULE
{


#define BaseModuleName "ModeModule"
#define BaseSCPIModuleName "MOD"

class cModeModuleConfiguration;
class cModeModuleInit;


class cModeModule : public cBaseMeasModule
{
Q_OBJECT

public:
    cModeModule(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, QObject* parent = nullptr);
    virtual QByteArray getConfiguration() const;

protected:
    cModeModuleInit *m_pModeModuleInit;
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

#endif // MODEMODULE_H
