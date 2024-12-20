#ifndef ADJUSTMENTMODULE_H
#define ADJUSTMENTMODULE_H

#include <basemeasmodule.h>

class cAdjustmentModuleMeasProgram;

enum dspInterfaceOrder { // For doc and test
    DSP_INTERFACE_RANGE_OBSERMATIC = 0,
    DSP_INTERFACE_RANGE_ADJUSTMENT,
    DSP_INTERFACE_RANGE_PROGRAM,
    DSP_INTERFACE_RMS,
    DSP_INTERFACE_DFT,
    DSP_INTERFACE_FFT,

    DSP_INTERFACE_COUNT
};

class cAdjustmentModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "AdjustmentModule";
    static constexpr const char* BaseSCPIModuleName = "ADJ";
    cAdjustmentModule(ModuleFactoryParam moduleParam);
    QByteArray getConfiguration() const override;
private slots:
    void activationDone() override;
    void activationFinished() override;

    void deactivationStart() override;
    void deactivationExec() override;
    void deactivationDone() override;
    void deactivationFinished() override;
private:
    cAdjustmentModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
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

#endif // ADJUSTMENTMODULE_H
