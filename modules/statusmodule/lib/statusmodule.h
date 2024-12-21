#ifndef STATUSMODULE_H
#define STATUSMODULE_H

#include "statusmoduleinit.h"
#include <basemeasmodule.h>
#include <QFinalState>

namespace STATUSMODULE
{

class cStatusModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "StatusModule";
    static constexpr const char* BaseSCPIModuleName = "DEV";

    cStatusModule(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;

private:
    cStatusModuleInit *m_pStatusModuleInit;
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

#endif // STATUSMODULE_H
