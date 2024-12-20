#ifndef MODEMODULE_H
#define MODEMODULE_H

#include "basemeasmodule.h"
#include <QFinalState>

namespace MODEMODULE {

class cModeModuleConfiguration;
class cModeModuleInit;

class cModeModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "ModeModule";
    static constexpr const char* BaseSCPIModuleName = "MOD";

    cModeModule(ModuleFactoryParam moduleParam);

private slots:
    void activationFinished() override;
private:
    cModeModuleInit *m_pModeModuleInit;
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // MODEMODULE_H
