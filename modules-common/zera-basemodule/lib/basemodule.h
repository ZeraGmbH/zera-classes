#ifndef BASEMODULE_H
#define BASEMODULE_H

#include "moduleactivist.h"
#include "basemoduleconfiguration.h"
#include "modulefactoryparam.h"
#include "vfmodulemetadata.h"
#include <vfmoduleactvalue.h>
#include <vfmoduleparameter.h>
#include <virtualmodule.h>
#include <xmlsettings.h>
#include <ve_eventsystem.h>
#include <vs_abstracteventsystem.h>

enum Status
{
    untouched,  // we just got new configuration data that is not yet analyzed
    setup,      // we have setup our interface
    activated   // we have activated the module
};


class BaseModule : public ZeraModules::VirtualModule
{
Q_OBJECT

public:
    BaseModule(ModuleFactoryParam moduleParam, std::shared_ptr<BaseModuleConfiguration> modcfg);
    virtual ~BaseModule();
    virtual void startModule() override;
    virtual void stopModule() override;
    void startDestroy() override;
    virtual void exportMetaData();

    VeinStorage::AbstractDatabase *getStorageDb() const;
    bool getDemo();
    const AbstractFactoryServiceInterfacesPtr getServiceInterfaceFactory() const;
    const ModuleNetworkParamsPtr getNetworkConfig() const;
    const ChannelRangeObserver::SystemObserverPtr getSharedChannelRangeObserver() const;

    QList<VfModuleMetaData*> veinModuleMetaDataList; // only meta information
    QList<VfModuleComponent*> veinModuleComponentList; // for components that need no scpi interface
    QList<VfModuleActvalue*> veinModuleActvalueList; // actvalues are components that need an interface
    QMap<QString, VfModuleParameter*> m_veinModuleParameterMap; // parameters are components that need an interface and validation
    QList<cSCPIInfo*> scpiCommandList; // a list of commands that work without existing component, it uses a component's validation data for additional queries

signals:
    void sigRun();
    void sigRunFailed();
    void sigStop();
    void sigStopFailed();
    void sigConfiguration(); // emitting this signal starts configuration
    void sigConfDone(); // emitted when configuration is done regardless of good or not
    void sigReconfigureContinue();

    void activationContinue();
    void activationNext();
    void deactivationContinue();
    void deactivationNext();

protected:
    virtual void setupModule(); // after xml configuration we can setup and export our module
    void unsetModule();
    virtual void startMeas() =  0;
    virtual void stopMeas() = 0;

    QStateMachine m_stateMachine;
    QStateMachine m_ActivationMachine; // we use statemachine for module activation
    QStateMachine m_DeactivationMachine; // and deactivation

    QState *m_pStateIdle;
    QState *m_pStateRun;
    QFinalState *m_pStateFinished;

    // additional states for IDLE
    QState* m_pStateIDLEIdle;
    QState* m_pStateIDLEConfSetup;

    // additional states for RUN
    QState* m_pStateRUNStart;
    QState* m_pStateRUNDone;
    QState* m_pStateRUNDeactivate;
    QState* m_pStateRUNConfSetup;

    // our states for base modules activation statemachine
    QState m_ActivationStartState;
    QState m_ActivationExecState;
    QState m_ActivationDoneState;
    QFinalState m_ActivationFinishedState;

    // our states for base modules deactivation statemachine
    QState m_DeactivationStartState;
    QState m_DeactivationExecState;
    QState m_DeactivationDoneState;
    QFinalState m_DeactivationFinishedState;

    QList<cModuleActivist*> m_ModuleActivistList;
    std::shared_ptr<BaseModuleConfiguration> m_pConfiguration; // our xml configuration
    QString m_sModuleDescription;

    VfModuleMetaData *m_pModuleName;
    VfModuleMetaData *m_pModuleDescription;
    VfModuleComponent *m_pModuleInterfaceComponent; // here we export the modules interface as json file
    VfModuleComponent *m_pModuleEntityName;

    VeinEvent::EventSystem *m_pModuleEventSystem;

protected slots:
    virtual void activationStart() = 0;
    virtual void activationExec() = 0;
    virtual void activationDone() = 0; // loop to exec or end up on finished
    virtual void activationFinished() = 0;

    virtual void deactivationStart() = 0;
    virtual void deactivationExec() = 0;
    virtual void deactivationDone() = 0;
    virtual void deactivationFinished() = 0;
private:
    const ModuleFactoryParam m_moduleParam;
    bool m_bConfCmd;
    bool m_bStartCmd;
    bool m_bStopCmd;
    bool m_bStateMachineStarted;

    int m_nStatus;
private slots:
    void entryIdle();
    void entryIDLEIdle();

    void entryConfSetup();

    void entryRunStart();
    void entryRunDone();
    void entryRunDeactivate();
};

#endif // BASEMODULE_H
