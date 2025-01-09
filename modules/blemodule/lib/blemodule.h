#ifndef BLEMODULE_H
#define BLEMODULE_H

#include "blemodulemeasprogram.h"
#include <basemeasmodule.h>

namespace BLEMODULE
{
class cBleModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "BleModule";
    static constexpr const char* BaseSCPIModuleName = "BLE";

    cBleModule(ModuleFactoryParam moduleParam);

private:
    cBleModuleMeasProgram *m_pMeasProgram = nullptr;
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // BLEMODULE_H
