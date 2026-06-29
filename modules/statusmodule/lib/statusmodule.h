#ifndef STATUSMODULE_H
#define STATUSMODULE_H

#include "statusmoduleconfiguration.h"
#include "statusmoduleinit.h"
#include <basemeasmodule.h>
#include <vfrpceventsystemsimplified.h>

namespace STATUSMODULE
{

class cStatusModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "StatusModule";
    static constexpr const char* BaseSCPIModuleName = "DEV";

    explicit cStatusModule(const ModuleFactoryParam &moduleParam);
    cStatusModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    VfRpcEventSystemSimplified *getRpcEventSystem();

private slots:
    void activationFinished() override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cStatusModuleInit *m_pStatusModuleInit = nullptr;
    cStatusModuleConfiguration m_configuration;
    std::unique_ptr<VfRpcEventSystemSimplified> m_spRpcEventSystem;
};

}

#endif // STATUSMODULE_H
