#ifndef OSCIMODULE_H
#define OSCIMODULE_H

#include "basemeasmodule.h"
#include <QFinalState>

namespace OSCIMODULE {

class cOsciModuleConfiguration;
class cOsciModuleMeasProgram;

class cOsciModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "OSCIModule";
    static constexpr const char* BaseSCPIModuleName = "OSC";

    cOsciModule(ModuleFactoryParam moduleParam);

private:
    cOsciModuleMeasProgram *m_pMeasProgram; // our measuring program, lets say the working horse
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // OSCIMODULE_H
