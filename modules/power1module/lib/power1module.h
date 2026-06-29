#ifndef POWER1MODULE_H
#define POWER1MODULE_H

#include "basemeasmodule.h"
#include "power1modulemeasprogram.h"
#include "power1moduleconfiguration.h"

namespace POWER1MODULE {

class cPower1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "POWER1Module";
    static constexpr const char* BaseSCPIModuleName = "POW";

    explicit cPower1Module(const ModuleFactoryParam &moduleParam);
    cPower1ModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cPower1ModuleMeasProgram *m_pMeasProgram = nullptr;
    cPower1ModuleConfiguration m_configuration;
};

}

#endif // POWER1MODULE_H
