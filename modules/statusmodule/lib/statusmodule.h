#ifndef STATUSMODULE_H
#define STATUSMODULE_H

#include "statusmoduleinit.h"
#include <basemeasmodule.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>

namespace STATUSMODULE
{

class cStatusModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "StatusModule";
    static constexpr const char* BaseSCPIModuleName = "DEV";

    cStatusModule(quint8 modnr, int entityId, VeinEvent::StorageSystem *storagesystem, bool demo);
    QByteArray getConfiguration() const override;

protected:
    cStatusModuleInit *m_pStatusModuleInit;
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

#endif // STATUSMODULE_H
