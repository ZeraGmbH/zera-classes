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

    explicit cStatusModule(ModuleFactoryParam moduleParam);

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cStatusModuleInit *m_pStatusModuleInit = nullptr;
};

}

#endif // STATUSMODULE_H
