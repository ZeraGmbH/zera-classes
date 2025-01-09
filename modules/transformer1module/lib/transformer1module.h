#ifndef TRANSFORMER1MODULE_H
#define TRANSFORMER1MODULE_H

#include "transformer1modulemeasprogram.h"
#include "basemeasmodule.h"

namespace TRANSFORMER1MODULE {

class cTransformer1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Transformer1Module";
    static constexpr const char* BaseSCPIModuleName = "TR1";

    explicit cTransformer1Module(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cTransformer1ModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // TRANSFORMER1MODULE_H
