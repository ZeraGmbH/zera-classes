#ifndef RMSMODULE_H
#define RMSMODULE_H

#include "rmsmodulemeasprogram.h"
#include <basemeasmodule.h>
#include <QFinalState>

namespace RMSMODULE
{
class cRmsModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RMSModule";
    static constexpr const char* BaseSCPIModuleName = "RMS";

    cRmsModule(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    TaskTemplatePtr getModuleSetUpTask() override;
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cRmsModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
};

}

#endif // RMSMODULE_H
