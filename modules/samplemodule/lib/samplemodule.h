#ifndef SAMPLEMODULE_H
#define SAMPLEMODULE_H

#include "samplemodulemeasprogram.h"
#include "samplechannel.h"
#include "pllmeaschannel.h"
#include "pllobsermatic.h"
#include <basemeasmodule.h>

namespace SAMPLEMODULE
{
class cSampleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SampleModule";
    static constexpr const char* BaseSCPIModuleName = "SAM";

    cSampleModule(ModuleFactoryParam moduleParam);
    cPllMeasChannel* getPllMeasChannel(const QString &name);

private slots:
    void activationFinished() override;
    void deactivationStart() override;
private:
    void setupModule() override; // after xml configuration we can setup and export our module
    TaskTemplatePtr getModuleSetUpTask() override;
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cSampleModuleMeasProgram *m_pMeasProgram = nullptr;
    cPllObsermatic *m_pPllObsermatic = nullptr;
    QList<cPllMeasChannel*> m_pllMeasChannelList; // our pll meas channels
    QList<cSampleChannel*>  m_sampleChannelList;
};

}

#endif // SAMPLEMODULE_H
