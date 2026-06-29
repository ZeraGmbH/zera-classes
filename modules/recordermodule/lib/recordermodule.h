#ifndef RECORDERMODULE_H
#define RECORDERMODULE_H

#include "recordermoduleconfiguration.h"
#include <basemeasmodule.h>
#include <vf_cmd_event_handler_system.h>
#include <vfrpceventsystemsimplified.h>

class RecorderModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RecorderModule";
    static constexpr const char* BaseSCPIModuleName = "REC";

    explicit RecorderModule(const ModuleFactoryParam &moduleParam);
    RecorderModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    VfRpcEventSystemSimplified *getRpcEventSystem();

private slots:
    void activationFinished() override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    RecorderModuleConfiguration m_configuration;
    std::unique_ptr<VfRpcEventSystemSimplified> m_spRpcEventSystem;
};

#endif // RECORDERMODULE_H
