#ifndef BURDEN1MODULE_H
#define BURDEN1MODULE_H

#include "burden1modulemeasprogram.h"
#include <basemeasmodule.h>

namespace BURDEN1MODULE
{
class cBurden1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Burden1Module";
    static constexpr const char* BaseSCPIModuleName = "BD1";

    cBurden1Module(ModuleFactoryParam moduleParam);

private:
    cBurden1ModuleMeasProgram *m_pMeasProgram = nullptr;
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // BURDEN1MODULE_H
