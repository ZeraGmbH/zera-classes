#ifndef DOSAGEMODULE_H
#define DOSAGEMODULE_H

#include "dosagemodulemeasprogram.h"
#include "dosagemoduleconfiguration.h"
#include <basemeasmodule.h>

namespace DOSAGEMODULE
{
class cDosageModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char *BaseModuleName = "DosageModule";
    static constexpr const char *BaseSCPIModuleName = "DOS";

    explicit cDosageModule(const ModuleFactoryParam &moduleParam);
    cDosageModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

protected:
    void setupModule() override;                            // after xml configuration we can setup and export our module
    void startMeas() override;                              // we make the measuring program start here
    void stopMeas() override;

    cDosageModuleMeasProgram *m_pMeasProgram = nullptr;     // our measuring program, lets say the working horse
    cDosageModuleConfiguration m_configuration;
};

}

#endif // DOSAGEMODULE_H
