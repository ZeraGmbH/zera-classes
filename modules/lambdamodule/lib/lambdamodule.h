#ifndef LAMBDAMODULE_H
#define LAMBDAMODULE_H

#include "lambdamodulemeasprogram.h"
#include <basemeasmodule.h>

namespace LAMBDAMODULE
{
class cLambdaModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "LambdaModule";
    static constexpr const char* BaseSCPIModuleName = "LAM";

    cLambdaModule(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
private slots:
    void activationExec() override;
    void activationDone() override;
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;
private:
    cLambdaModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // LAMBDAMODULE_H
