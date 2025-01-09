#ifndef POWER2MODULE_H
#define POWER2MODULE_H

#include "basemeasmodule.h"

namespace POWER2MODULE
{

class cPower2ModuleConfiguration;
class cPower2ModuleMeasProgram;

class cPower2Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "POWER2Module";
    static constexpr const char* BaseSCPIModuleName = "PW2";

    cPower2Module(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    TaskTemplatePtr getModuleSetUpTask() override;
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cPower2ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
};

}

#endif // POWER2MODULE_H
