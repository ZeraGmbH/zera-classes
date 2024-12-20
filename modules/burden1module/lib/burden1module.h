#ifndef BURDEN1MODULE_H
#define BURDEN1MODULE_H

#include "burden1modulemeasprogram.h"
#include <basemeasmodule.h>

namespace BURDEN1MODULE
{
class cBurden1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Burden1Module";
    static constexpr const char* BaseSCPIModuleName = "BD1";

    cBurden1Module(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

private slots:
    void activationDone() override;
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;
private:
    cBurden1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // BURDEN1MODULE_H
