#ifndef SAMPLEMODULE_H
#define SAMPLEMODULE_H

#include "samplemodulemeasprogram.h"
#include "samplechannel.h"
#include "pllmeaschannel.h"
#include "pllobsermatic.h"
#include <basemeasmodule.h>

namespace SAMPLEMODULE
{
class cSampleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SampleModule";
    static constexpr const char* BaseSCPIModuleName = "SAM";

    cSampleModule(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
    cPllMeasChannel* getPllMeasChannel(QString name);

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
    cSampleModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    cPllObsermatic *m_pPllObsermatic; // our pll handling

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

    QList<cPllMeasChannel*> m_pllMeasChannelList; // our pll meas channels
    QList<cSampleChannel*>  m_sampleChannelList;
};

}

#endif // SAMPLEMODULE_H
