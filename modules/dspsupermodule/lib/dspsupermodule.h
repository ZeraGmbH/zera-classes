#ifndef DSPSUPERMODULE_H
#define DSPSUPERMODULE_H

#include "basemeasmodule.h"

namespace DSPSUPERMODULE {

class DspSuperModuleMeasProgram;

class DspSuperModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "DspSuperModule";
    static constexpr const char* BaseSCPIModuleName = "DSP";

    DspSuperModule(const ModuleFactoryParam &moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
    DspSuperModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // DSPSUPERMODULE_H
