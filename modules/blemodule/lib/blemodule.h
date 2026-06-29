#ifndef BLEMODULE_H
#define BLEMODULE_H

#include "blemodulemeasprogram.h"
#include "blemoduleconfiguration.h"
#include <basemeasmodule.h>

namespace BLEMODULE
{
class cBleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "BleModule";
    static constexpr const char* BaseSCPIModuleName = "BLE";

    explicit cBleModule(const ModuleFactoryParam &moduleParam);
    cBleModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cBleModuleMeasProgram *m_pMeasProgram = nullptr;
    cBleModuleConfiguration m_configuration;
};

}

#endif // BLEMODULE_H
