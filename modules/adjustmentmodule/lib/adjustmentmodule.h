#ifndef ADJUSTMENTMODULE_H
#define ADJUSTMENTMODULE_H

#include "adjustmentmodulemeasprogram.h"
#include <basemeasmodule.h>

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

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    TaskTemplatePtr getModuleSetUpTask() override;
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cAdjustmentModuleMeasProgram *m_pMeasProgram = nullptr;
};

#endif // ADJUSTMENTMODULE_H
