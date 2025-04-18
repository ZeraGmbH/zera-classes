#ifndef POWER1MODULE_H
#define POWER1MODULE_H

#include "power1modulemeasprogram.h"
#include "basemeasmodule.h"

namespace POWER1MODULE {

class cPower1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "POWER1Module";
    static constexpr const char* BaseSCPIModuleName = "POW";

    cPower1Module(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cPower1ModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // POWER1MODULE_H
