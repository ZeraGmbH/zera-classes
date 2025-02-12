#ifndef DFTMODULE_H
#define DFTMODULE_H

#include "basemeasmodule.h"

namespace DFTMODULE {

class cDftModuleMeasProgram;

class cDftModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "DFTModule";
    static constexpr const char* BaseSCPIModuleName = "DFT";

    cDftModule(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
    cDftModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // DFTMODULE_H
