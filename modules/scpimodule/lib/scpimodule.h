#ifndef SCPIMODULE_H
#define SCPIMODULE_H

#include "scpiveintransactioninfo.h"
#include "scpiserver.h"
#include "scpimoduleconfiguration.h"
#include "signalconnectiondelegate.h"
#include "veincomponentscpimeasuresequence.h"
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

    explicit cSCPIModule(const ModuleFactoryParam &moduleParam);
    ~cSCPIModule();
    cSCPIServer* getSCPIServer();

    cSCPIModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    void removeClient(cSCPIClient *client);

    QMultiHash<QString, SCPIVeinTransactionInfoPtr> scpiParameterCmdInfoHash; // a hash to memorize what was in progress for which client
    QList<cSignalConnectionDelegate*> sConnectDelegateList;
    std::shared_ptr<QMultiHash<QString /* vein component or Rpc name */,
                               VeinComponentScpiMeasureSequence*>> m_moduleCommonPendingMeasureStore;

    VfEventSytemModuleParam *getValidatorEventSystem();
    SCPIEventSystem* m_pSCPIEventSystem = nullptr;
    VfCmdEventHandlerSystemPtr getCmdEventHandlerSystem();

private slots:
    void activationFinished() override;
private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    cSCPIModuleConfiguration m_configuration;
    cSCPIServer *m_pSCPIServer = nullptr;
    VfEventSytemModuleParam* m_pModuleValidator = nullptr;
    VfCmdEventHandlerSystemPtr m_cmdEventHandlerSystem;
};

}

#endif // SCPIMODULE_H
