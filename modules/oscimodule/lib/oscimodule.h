#ifndef OSCIMODULE_H
#define OSCIMODULE_H

#include "basemeasmodule.h"

namespace OSCIMODULE {

class cOsciModuleMeasProgram;

class cOsciModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "OSCIModule";
    static constexpr const char* BaseSCPIModuleName = "OSC";

    cOsciModule(const ModuleFactoryParam &moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cOsciModuleMeasProgram *m_pMeasProgram = nullptr;
};

}

#endif // OSCIMODULE_H
