#ifndef SFCMODULE_H
#define SFCMODULE_H

#include "sfcmoduleconfiguration.h"
#include "sfcmodulemeasprogram.h"
#include <basemeasmodule.h>

namespace SFCMODULE
{
class cSfcModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char *BaseModuleName = "SfcModule";
    static constexpr const char *BaseSCPIModuleName = "SFC";

    explicit cSfcModule(const ModuleFactoryParam &moduleParam);
    cSfcModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    virtual void setupModule() override;
    virtual void startMeas() override;
    virtual void stopMeas() override;

    cSfcModuleMeasProgram *m_pMeasProgram = nullptr;
    cSfcModuleConfiguration m_configuration;
};

}

#endif // SFCMODULE_H
