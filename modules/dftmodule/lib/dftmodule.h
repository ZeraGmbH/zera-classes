#ifndef DFTMODULE_H
#define DFTMODULE_H

#include "basemeasmodule.h"
#include <QFinalState>

namespace DFTMODULE {

class cDftModuleConfiguration;
class cDftModuleMeasProgram;

class cDftModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "DFTModule";
    static constexpr const char* BaseSCPIModuleName = "DFT";

    cDftModule(ModuleFactoryParam moduleParam);

private:
    cDftModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // DFTMODULE_H
