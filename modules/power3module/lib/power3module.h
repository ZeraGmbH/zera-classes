#ifndef POWER3MODULE_H
#define POWER3MODULE_H

#include "power3modulemeasprogram.h"
#include <basemeasmodule.h>

class cModuleError;

namespace POWER3MODULE
{
class cPower3Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Power3Module";
    static constexpr const char* BaseSCPIModuleName = "PW3";

    cPower3Module(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

private slots:
    void activationFinished() override;
    void deactivationFinished() override;
private:
    cPower3ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // POWER3MODULE_H
