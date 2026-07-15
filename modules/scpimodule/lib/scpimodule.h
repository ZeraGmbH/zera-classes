#ifndef SCPIMODULE_H
#define SCPIMODULE_H

#include "scpiveintransactioninfo.h"
#include "basemodule.h"
#include "scpiserver.h"
#include "scpimoduleconfiguration.h"
#include "scpimodeldeviface.h"
#include "scpimodelstatus.h"
#include "scpimodelieee4882.h"
#include "scpimodelrpcs.h"
#include "scpimodelcatalogs.h"
#include "signalconnectiondelegate.h"
#include "veinscpimoduleinterfaceparser.h"
#include "vfeventsytemmoduleparam.h"
#include <vf_cmd_event_handler_system.h>

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
    cSCPIInterface* getScpiInterface();
    cSCPIServer* getSCPIServer();

    cSCPIModuleConfigData *getConfigData();
    QByteArray getConfigXml() const override;

    ScpiModelMeasureAndFriends *getScpiModelMeasurement();
    ScpiModelCatalogs* getScpiModelCatalogs();

    void removeClientParamOrRpcTransactions(cSCPIClient *client);

    void insertScpiVeinParamRpcTransaction(const QString &componentOrRpcName,
                                           const SCPIVeinTransactionInfoPtr &transactionInfo);
    void removeScpiVeinParamRpcTransaction(const QString &componentOrRpcName,
                                           const SCPIVeinTransactionInfoPtr &transactionInfo);
    const QList<SCPIVeinTransactionInfoPtr> getAllScpiVeinParamRpcTransactions();
    const QList<SCPIVeinTransactionInfoPtr> getScpiVeinParamRpcTransactions(const QString &componentOrRpcName);

    void insertSignalConnectionDelegate(cSignalConnectionDelegate* delegate);
    void removeSignalConnectionDelegates(cSignalConnectionDelegate *delegate);
    void updateSignalConnection(int entityId, const QString &componentName, const QVariant &newValue);

    void updatePendingMeasureSequences(int entityId, const QString &componentName, const QVariant &newValue);

    VfEventSytemModuleParam *getValidatorEventSystem();
    void emitSigSendEvent(QEvent *event);
    VfCmdEventHandlerSystemPtr getCmdEventHandlerSystem();

    const VeinScpiModuleInterfaceParser &getScpiModuleInterfaceParser() const;

private slots:
    void activationFinished() override;
private:
    void setupModule() override; // after xml configuration we can setup and export our module
    void startMeas() override; // we make the measuring program start here
    void stopMeas() override;

    SCPIEventSystem* m_pSCPIEventSystem = nullptr;
    cSCPIModuleConfiguration m_configuration;
    cSCPIServer *m_pSCPIServer = nullptr;
    VfEventSytemModuleParam* m_pModuleValidator = nullptr;
    VfCmdEventHandlerSystemPtr m_cmdEventHandlerSystem;
    VeinScpiModuleInterfaceParser m_moduleInterfaceParser;

    cSCPIInterface m_scpiInterface;
    ScpiModelMeasureAndFriends m_scpiModelMeasurement;
    ScpiModelParameters m_scpiModelParameters;
    ScpiModelCatalogs m_scpiModelCatalogs;
    ScpiModelRpcs m_scpiModelRpcs;
    ScpiModelDevIface m_scpiModelDevIface;
    ScpiModelStatus m_scpiModelStatus;
    ScpiModelIEEE4882 m_scpiModelIeee488;

    QMultiHash<QString /*componentOrRpcName*/, SCPIVeinTransactionInfoPtr> m_scpiVeinParamOrRpcTransactions;
    QList<cSignalConnectionDelegate*> m_signalConnectionDelegates;
};

}

#endif // SCPIMODULE_H
