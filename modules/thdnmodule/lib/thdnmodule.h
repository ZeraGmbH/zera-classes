#ifndef THDNMODULE_H
#define THDNMODULE_H

#include "basemeasmodule.h"
#include <QFinalState>

namespace THDNMODULE {

class cThdnModuleConfiguration;
class cThdnModuleMeasProgram;

class cThdnModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "THDNModule";
    static constexpr const char* BaseSCPIModuleName = "THD";

    cThdnModule(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

private slots:
    void activationFinished() override;
    void deactivationFinished() override;
private:
    cThdnModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // THDNMODULE_H
