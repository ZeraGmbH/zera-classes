#ifndef SEM1MODULE_H
#define SEM1MODULE_H

#include "sem1modulemeasprogram.h"
#include <basemeasmodule.h>
#include <vfrpceventsystemsimplified.h>

namespace SEM1MODULE
{
class cSem1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SEM1Module";
    // shortcut of scpi module name is only first 4 characters
    // so we will provide energy measurement type EM01 .. EM99
    // and each energy measurement  will have an additional scpi parent with its number 0001 .. 9999
    static constexpr const char* BaseSCPIModuleName = "EM01";

    cSem1Module(ModuleFactoryParam moduleParam);
    VfRpcEventSystemSimplified *getRpcEventSystem();

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cSem1ModuleMeasProgram *m_pMeasProgram = nullptr;
    std::shared_ptr<VfRpcEventSystemSimplified> m_spRpcEventSystem;
};

}

#endif // SEM1MODULE_H
