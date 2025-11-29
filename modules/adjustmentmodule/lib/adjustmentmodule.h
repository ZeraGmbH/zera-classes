#ifndef ADJUSTMENTMODULE_H
#define ADJUSTMENTMODULE_H

#include "adjustmentmodulemeasprogram.h"
#include <basemeasmodule.h>

class cAdjustmentModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "AdjustmentModule";
    static constexpr const char* BaseSCPIModuleName = "ADJ";
    cAdjustmentModule(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cAdjustmentModuleMeasProgram *m_pMeasProgram = nullptr;
};

#endif // ADJUSTMENTMODULE_H
