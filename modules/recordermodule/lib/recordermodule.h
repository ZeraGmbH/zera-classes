#ifndef RECORDERMODULE_H
#define RECORDERMODULE_H

#include <basemeasmodule.h>
#include <vf_cmd_event_handler_system.h>
#include <vfrpceventsystemsimplified.h>

class RecorderModule : public cBaseMeasModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RecorderModule";
    static constexpr const char* BaseSCPIModuleName = "REC";

    RecorderModule(ModuleFactoryParam moduleParam);

    VfRpcEventSystemSimplified *getRpcEventSystem();

private slots:
    void activationFinished() override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    std::unique_ptr<VfRpcEventSystemSimplified> m_spRpcEventSystem;
};

#endif // RECORDERMODULE_H
