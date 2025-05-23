#ifndef SPM1MODULE_H
#define SPM1MODULE_H

#include "spm1modulemeasprogram.h"
#include <basemeasmodule.h>

namespace SPM1MODULE
{
class cSpm1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SPM1Module";
    // shortcut of scpi module name is only first 4 characters
    // so we will provide energy measurement type PM01 .. PM99
    // and each energy measurement  will have an additional scpi parent with its number 0001 .. 9999
    static constexpr const char* BaseSCPIModuleName = "PM01";

    explicit cSpm1Module(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cSpm1ModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // SPM1MODULE_H
