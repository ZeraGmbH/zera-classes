#ifndef TRANSFORMER1MODULE_H
#define TRANSFORMER1MODULE_H

#include "basemeasmodule.h"
#include <QFinalState>
#include <QList>

namespace TRANSFORMER1MODULE {

class cTransformer1ModuleConfiguration;
class cTransformer1ModuleMeasProgram;
class cTransformer1ModuleObservation;

class cTransformer1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Transformer1Module";
    static constexpr const char* BaseSCPIModuleName = "TR1";

    cTransformer1Module(ModuleFactoryParam moduleParam);
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
    cTransformer1ModuleObservation *m_pTransformer1ModuleObservation;
    cTransformer1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // TRANSFORMER1MODULE_H
