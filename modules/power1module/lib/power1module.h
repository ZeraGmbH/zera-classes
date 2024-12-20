#ifndef POWER1MODULE_H
#define POWER1MODULE_H

#include "basemeasmodule.h"
#include <QFinalState>
#include <memory>

namespace POWER1MODULE {

class cPower1ModuleConfiguration;
class cPower1ModuleMeasProgram;

class cPower1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "POWER1Module";
    static constexpr const char* BaseSCPIModuleName = "POW";

    cPower1Module(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
private slots:
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;
private:
    cPower1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // POWER1MODULE_H
