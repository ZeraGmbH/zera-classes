#ifndef THDNMODULE_H
#define THDNMODULE_H

#include "basemeasmodule.h"
#include "basemeasprogram.h"
#include "thdnmoduleconfiguration.h"

namespace THDNMODULE {

class cThdnModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "THDNModule";
    static constexpr const char* BaseSCPIModuleName = "THD";

    explicit cThdnModule(const ModuleFactoryParam &moduleParam);
    cThdnModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cBaseMeasProgram *m_pMeasProgram = nullptr;
    cThdnModuleConfiguration m_configuration;
};

}

#endif // THDNMODULE_H
