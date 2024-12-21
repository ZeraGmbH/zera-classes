#ifndef STATUSMODULE_H
#define STATUSMODULE_H

#include "statusmoduleinit.h"
#include <basemeasmodule.h>

namespace STATUSMODULE
{

class cStatusModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "StatusModule";
    static constexpr const char* BaseSCPIModuleName = "DEV";

    cStatusModule(ModuleFactoryParam moduleParam);

private:
    cStatusModuleInit *m_pStatusModuleInit;
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
};

}

#endif // STATUSMODULE_H
