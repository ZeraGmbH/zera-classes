#ifndef POWER3MODULE_H
#define POWER3MODULE_H

#include "power3moduleconfiguration.h"
#include "power3modulemeasprogram.h"
#include <basemeasmodule.h>

namespace POWER3MODULE
{
class cPower3Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Power3Module";
    static constexpr const char* BaseSCPIModuleName = "PW3";

    explicit cPower3Module(const ModuleFactoryParam &moduleParam);
    cPower3ModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cPower3ModuleMeasProgram *m_pMeasProgram = nullptr;
    cPower3ModuleConfiguration m_configuration;
};

}

#endif // POWER3MODULE_H
