#ifndef APIMODULE_H
#define APIMODULE_H

#include "vfeventsytemmoduleparam.h"
#include <basemodule.h>

namespace APIMODULE
{
class cApiModule : public BaseModule
{
    Q_OBJECT
public:
    static constexpr const char *BaseModuleName = "ApiModule";
    static constexpr const char *BaseSCPIModuleName = "API";

    cApiModule(ModuleFactoryParam moduleParam);

private slots:
    void activationFinished() override;
protected:
    virtual void setupModule();                             // after xml configuration we can setup and export our module
    virtual void startMeas();                               // we make the measuring program start here
    virtual void stopMeas();

    VfEventSytemModuleParam* m_pModuleValidator;
};

}

#endif // APIMODULE_H
