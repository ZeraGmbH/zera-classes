#ifndef LAMBDAMODULE_H
#define LAMBDAMODULE_H

#include "lambdamoduleconfiguration.h"
#include "lambdamodulemeasprogram.h"
#include <basemeasmodule.h>

namespace LAMBDAMODULE
{
class cLambdaModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "LambdaModule";
    static constexpr const char* BaseSCPIModuleName = "LAM";

    explicit cLambdaModule(const ModuleFactoryParam &moduleParam);
    cLambdaModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cLambdaModuleMeasProgram *m_pMeasProgram = nullptr;
    cLambdaModuleConfiguration m_configuration;
};

}

#endif // LAMBDAMODULE_H
