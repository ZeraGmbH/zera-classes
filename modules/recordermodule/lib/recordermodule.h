#ifndef RECORDERMODULE_H
#define RECORDERMODULE_H

#include <basemodule.h>
#include <vfrpceventsystemsimplified.h>
#include "vfeventsytemmoduleparam.h"

class RecorderModule : public BaseModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "RecorderModule";
    static constexpr const char* BaseSCPIModuleName = "REC";

    RecorderModule(ModuleFactoryParam moduleParam);
    VfRpcEventSystemSimplified *getRpcEventSystem();
    VfEventSytemModuleParam* getValidatorEventSystem();

private slots:
    void activationFinished() override;

private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    std::shared_ptr<VfEventSytemModuleParam> m_spModuleValidator;
    std::shared_ptr<VfRpcEventSystemSimplified> m_spRpcEventSystem;

};

#endif // RECORDERMODULE_H
