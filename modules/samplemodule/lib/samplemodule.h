#ifndef SAMPLEMODULE_H
#define SAMPLEMODULE_H

#include "samplemodulemeasprogram.h"
#include "samplemoduleconfiguration.h"
#include <basemeasmodule.h>

namespace SAMPLEMODULE
{
class cSampleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SampleModule";
    static constexpr const char* BaseSCPIModuleName = "SAM";

    explicit cSampleModule(const ModuleFactoryParam &moduleParam);
    cSampleModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private slots:
    void activationFinished() override;
    void deactivationStart() override;
private:
    void setupModule() override;
    void startMeas() override { /* no start/stop */ };
    void stopMeas() override  { /* no start/stop */ };

    cSampleModuleMeasProgram *m_pMeasProgram = nullptr;
    cSampleModuleConfiguration m_configuration;
};

}

#endif // SAMPLEMODULE_H
