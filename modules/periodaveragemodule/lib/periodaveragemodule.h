#ifndef PERIODAVERAGEMODULE_H
#define PERIODAVERAGEMODULE_H

#include "basemeasmodule.h"
#include "periodaveragemoduleconfiguration.h"

namespace PERIODAVERAGEMODULE {

class PeriodAverageModuleMeasProgram;

class PeriodAverageModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "PeriodAverageModule";
    static constexpr const char* BaseSCPIModuleName = "PAV";

    explicit PeriodAverageModule(const ModuleFactoryParam &moduleParam);
    PeriodAverageModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    PeriodAverageModuleMeasProgram *m_pMeasProgram = nullptr;
    PeriodAverageModuleConfiguration m_configuration;
};

}

#endif // PERIODAVERAGEMODULE_H
