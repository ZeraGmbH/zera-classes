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

    void insertScpiVeinParamRpcTransaction(const QString &componentOrRpcName,
                                           const SCPIVeinTransactionInfoPtr &transactionInfo);
    void removeScpiVeinParamRpcTransaction(const QString &componentOrRpcName,
                                           const SCPIVeinTransactionInfoPtr &transactionInfo);
    const QList<SCPIVeinTransactionInfoPtr> getAllScpiVeinParamRpcTransactions();
    const QList<SCPIVeinTransactionInfoPtr> getScpiVeinParamRpcTransactions(const QString &componentOrRpcName);

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
    QMultiHash<QString /*componentOrRpcName*/, SCPIVeinTransactionInfoPtr> m_scpiVeinParamOrRpcTransactions;
};

}

#endif // SCPIMODULE_H
