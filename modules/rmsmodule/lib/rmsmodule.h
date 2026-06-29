#ifndef RMSMODULE_H
#define RMSMODULE_H

#include "rmsmoduleconfiguration.h"
#include <basemeasmodule.h>

namespace RMSMODULE
{
class cRmsModuleMeasProgram;

class cRmsModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RMSModule";
    static constexpr const char* BaseSCPIModuleName = "RMS";

    explicit cRmsModule(const ModuleFactoryParam &moduleParam);
    cRmsModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cRmsModuleMeasProgram *m_pMeasProgram = nullptr;
    cRmsModuleConfiguration m_configuration;
};

}

#endif // RMSMODULE_H
