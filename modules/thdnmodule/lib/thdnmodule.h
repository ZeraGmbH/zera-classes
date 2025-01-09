#ifndef THDNMODULE_H
#define THDNMODULE_H

#include "thdnmodulemeasprogram.h"
#include "basemeasmodule.h"

namespace THDNMODULE {

class cThdnModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "THDNModule";
    static constexpr const char* BaseSCPIModuleName = "THD";

    explicit cThdnModule(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    TaskTemplatePtr getModuleSetUpTask() override;
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cThdnModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // THDNMODULE_H
