#ifndef RMSMODULE_H
#define RMSMODULE_H

#include "rmsmodulemeasprogram.h"
#include <basemeasmodule.h>
#include <QFinalState>

namespace RMSMODULE
{
class cRmsModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RMSModule";
    static constexpr const char* BaseSCPIModuleName = "RMS";

    cRmsModule(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

private slots:
    void activationFinished() override;

    void deactivationDone() override;
    void deactivationFinished() override;
private:
    cRmsModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // RMSMODULE_H
