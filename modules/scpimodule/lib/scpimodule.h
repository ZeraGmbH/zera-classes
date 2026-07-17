#ifndef SCPIMODULE_H
#define SCPIMODULE_H

#include "basemodule.h"
#include "scpiserver.h"
#include "scpimoduleconfiguration.h"
#include "scpimodeldeviface.h"
#include "scpimodelstatus.h"
#include "scpimodelieee4882.h"
#include "scpimodelrpcs.h"
#include "scpimodelcatalogs.h"
#include "signalconnectiondelegateupdater.h"
#include "scpirpctransactionstore.h"
#include "scpiveinparser.h"
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

    void updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue);
    ScpiModelCatalogs* getScpiModelCatalogs();
    const ScpiVeinParser &getScpiModuleInterfaceParser() const;
    SignalConnectionDelegateUpdater *getSignalDelegateUpdater();
    ScpiRpcTransactionStore* getRpcTransactionStore();

    VfEventSytemModuleParam *getValidatorEventSystem();
    VfCmdEventHandlerSystemPtr getCmdEventHandlerSystem();
    void emitSigSendEvent(QEvent *event);

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
    ScpiVeinParser m_moduleInterfaceParser;

    cSCPIInterface m_scpiInterface;
    ScpiModelMeasure m_scpiModelMeasurement;
    ScpiModelParameters m_scpiModelParameters;
    ScpiModelCatalogs m_scpiModelCatalogs;
    ScpiModelRpcs m_scpiModelRpcs;
    ScpiModelDevIface m_scpiModelDevIface;
    ScpiModelStatus m_scpiModelStatus;
    ScpiModelIEEE4882 m_scpiModelIeee488;

    SignalConnectionDelegateUpdater m_signalDelegateUpdater;
    ScpiRpcTransactionStore m_rpcTransactionStore;
};

}

#endif // SCPIMODULE_H
