#ifndef POWER2MODULE_H
#define POWER2MODULE_H

#include "basemeasmodule.h"
#include "power2moduleconfiguration.h"

namespace POWER2MODULE
{

class cPower2ModuleMeasProgram;

class cPower2Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "POWER2Module";
    static constexpr const char* BaseSCPIModuleName = "PW2";

    explicit cPower2Module(const ModuleFactoryParam &moduleParam);
    cPower2ModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cPower2ModuleMeasProgram *m_pMeasProgram = nullptr;
    cPower2ModuleConfiguration m_configuration;
};

}

#endif // POWER2MODULE_H
