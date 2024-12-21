#ifndef FFTMODULE_H
#define FFTMODULE_H

#include "basemeasmodule.h"
#include <QFinalState>

namespace FFTMODULE {

class cFftModuleConfiguration;
class cFftModuleMeasProgram;

class cFftModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "FFTModule";
    static constexpr const char* BaseSCPIModuleName = "FFT";

    cFftModule(ModuleFactoryParam moduleParam);

private:
    cFftModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // FFTMODULE_H
