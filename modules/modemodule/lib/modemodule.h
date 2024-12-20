#ifndef MODEMODULE_H
#define MODEMODULE_H

#include "basemeasmodule.h"
#include <QFinalState>

namespace MODEMODULE {

class cModeModuleConfiguration;
class cModeModuleInit;

class cModeModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "ModeModule";
    static constexpr const char* BaseSCPIModuleName = "MOD";

    cModeModule(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

private slots:
    void activationStart() override;
    void activationExec() override;
    void activationDone() override;
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;
private:
    cModeModuleInit *m_pModeModuleInit;
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

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
};

}

#endif // MODEMODULE_H
