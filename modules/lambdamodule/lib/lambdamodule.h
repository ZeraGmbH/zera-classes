#ifndef LAMBDAMODULE_H
#define LAMBDAMODULE_H

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

    cLambdaModule(ModuleFactoryParam moduleParam);

private:
    cLambdaModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // LAMBDAMODULE_H
