#ifndef APIMODULE_H
#define APIMODULE_H

#include "vfeventsytemmoduleparam.h"
#include <basemodule.h>
#include <vfmodulerpc.h>

namespace APIMODULE
{
class cApiModule : public BaseModule
{
    Q_OBJECT
public:
    static constexpr const char *BaseModuleName = "ApiModule";
    static constexpr const char *BaseSCPIModuleName = "API";

    VfEventSytemModuleParam* getValidatorEventSystem();
    VfModuleRpc *getRpcEventSystem() const;

    cApiModule(ModuleFactoryParam moduleParam);
    ~cApiModule();

private slots:
    void activationFinished() override;
protected:
    void setupModule() override;                             // after xml configuration we can setup and export our module
    void startMeas() override;                               // we make the measuring program start here
    void stopMeas() override;

    VfEventSytemModuleParam* m_pModuleValidator = nullptr;
    VfModuleRpc *m_rpcEventSystem = nullptr;
};

}

#endif // APIMODULE_H
