#ifndef TRANSFORMER1MODULE_H
#define TRANSFORMER1MODULE_H

#include "basemeasmodule.h"
#include <QFinalState>
#include <QList>

namespace TRANSFORMER1MODULE {

class cTransformer1ModuleConfiguration;
class cTransformer1ModuleMeasProgram;
class cTransformer1ModuleObservation;

class cTransformer1Module : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "Transformer1Module";
    static constexpr const char* BaseSCPIModuleName = "TR1";

    cTransformer1Module(ModuleFactoryParam moduleParam);

private:
    cTransformer1ModuleObservation *m_pTransformer1ModuleObservation;
    cTransformer1ModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // TRANSFORMER1MODULE_H
