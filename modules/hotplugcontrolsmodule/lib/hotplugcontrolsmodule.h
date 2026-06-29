#ifndef HOTPLUGCONTROLSMODULE_H
#define HOTPLUGCONTROLSMODULE_H

#include "hotplugcontrolsmoduleconfiguration.h"
#include "vfeventsytemmoduleparam.h"
#include <basemodule.h>
#include <vfrpceventsystemsimplified.h>

namespace HOTPLUGCONTROLSMODULE
{

class cHotplugControlsModule : public BaseModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "HotplugControlsModule";
    static constexpr const char* BaseSCPIModuleName = "HOTP";

    explicit cHotplugControlsModule(const ModuleFactoryParam &moduleParam);
    cHotplugControlsModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    VfRpcEventSystemSimplified *getRpcEventSystem();
    VfEventSytemModuleParam* getValidatorEventSystem();

private slots:
    void activationFinished() override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cHotplugControlsModuleConfiguration m_configuration;
    std::shared_ptr<VfEventSytemModuleParam> m_spModuleValidator;
    std::shared_ptr<VfRpcEventSystemSimplified> m_spRpcEventSystem;
};

}

#endif // HOTPLUGCONTROLSMODULE_H
