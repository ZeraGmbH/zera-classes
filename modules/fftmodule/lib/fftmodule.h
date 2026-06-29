#ifndef FFTMODULE_H
#define FFTMODULE_H

#include "basemeasmodule.h"
#include "fftmoduleconfiguration.h"

namespace FFTMODULE {

class cFftModuleMeasProgram;

class cFftModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "FFTModule";
    static constexpr const char* BaseSCPIModuleName = "FFT";

    explicit cFftModule(const ModuleFactoryParam &moduleParam);
    cFftModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cFftModuleMeasProgram *m_pMeasProgram = nullptr;
    cFftModuleConfiguration m_configuration;
};

}

#endif // FFTMODULE_H
