#ifndef THDNMODULE_H
#define THDNMODULE_H

#include "basemeasmodule.h"

namespace THDNMODULE {

class cThdnModuleConfiguration;
class cThdnModuleMeasProgram;

class cThdnModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "THDNModule";
    static constexpr const char* BaseSCPIModuleName = "THD";

    cThdnModule(ModuleFactoryParam moduleParam);

private:
    cThdnModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // THDNMODULE_H
