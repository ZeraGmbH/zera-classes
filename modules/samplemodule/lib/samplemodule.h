#ifndef SAMPLEMODULE_H
#define SAMPLEMODULE_H

#include "samplemodulemeasprogram.h"
#include <basemeasmodule.h>

namespace SAMPLEMODULE
{
class cSampleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SampleModule";
    static constexpr const char* BaseSCPIModuleName = "SAM";

    explicit cSampleModule(ModuleFactoryParam moduleParam);

private slots:
    void activationFinished() override;
    void deactivationStart() override;
private:
    void setupModule() override;
    void startMeas() override { /* no start/stop */ };
    void stopMeas() override  { /* no start/stop */ };

    cSampleModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // SAMPLEMODULE_H
