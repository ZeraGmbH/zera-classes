#ifndef SCPIMODULE_H
#define SCPIMODULE_H

#include "scpiclientinfo.h"
#include "scpiserver.h"
#include "signalconnectiondelegate.h"
#include "scpimeasure.h"
#include "basemodule.h"
#include "vfeventsytemmoduleparam.h"
#include <vf_cmd_event_handler_system.h>
#include <memory>

namespace SCPIMODULE {

class SCPIEventSystem;

class cSCPIModule : public BaseModule
{
    Q_OBJECT
public:
    static constexpr const char* BaseModuleName = "SCPIModule";
    static constexpr const char* BaseSCPIModuleName = "SCP";

    cSCPIModule(ModuleFactoryParam moduleParam);
    ~cSCPIModule();
    cSCPIServer* getSCPIServer();

    QMultiHash<QString, SCPIClientInfoPtr> scpiParameterCmdInfoHash; // a hash to memorize what was in progress for which client
    QList<cSignalConnectionDelegate*> sConnectDelegateList;
    std::shared_ptr<QMultiHash<QString, cSCPIMeasure*>> m_scpiMeasureHash;

    VfEventSytemModuleParam *getValidatorEventSystem();
    SCPIEventSystem* m_pSCPIEventSystem;
    VfCmdEventHandlerSystemPtr getCmdEventHandlerSystem();

protected:
    cSCPIServer *m_pSCPIServer = nullptr;

private slots:
    void activationFinished() override;
private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;
    VfEventSytemModuleParam* m_pModuleValidator;
    VfCmdEventHandlerSystemPtr m_cmdEventHandlerSystem;
};

}

#endif // SCPIMODULE_H
