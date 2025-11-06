#ifndef HOTPLUGCONTROLSMODULE_H
#define HOTPLUGCONTROLSMODULE_H

#include "vfeventsytemmoduleparam.h"
#include <basemodule.h>

namespace HOTPLUGCONTROLSMODULE
{

class cHotplugControlsModule : public BaseModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "HotplugControlsModule";
    static constexpr const char* BaseSCPIModuleName = "HOT";

    explicit cHotplugControlsModule(ModuleFactoryParam moduleParam);
private slots:
    void activationFinished() override;
private:
    VfEventSytemModuleParam* getValidatorEventSystem();
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    std::shared_ptr<VfEventSytemModuleParam> m_spModuleValidator;
};

}

#endif // HOTPLUGCONTROLSMODULE_H
