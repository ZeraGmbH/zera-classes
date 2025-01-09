#ifndef BASEMODULE_H
#define BASEMODULE_H

#include "moduleactivist.h"
#include "basemoduleconfiguration.h"
#include "modulefactoryparam.h"
#include "vfmodulemetadata.h"
#include <vfmoduleactvalue.h>
#include <vfmoduleparameter.h>
#include <virtualmodule.h>
#include <vs_abstracteventsystem.h>

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

    QByteArray getConfiguration() const override;
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
    // signals emitted by modules
    void activationContinue();
    void activationNext();
    void activationReady();

    void deactivationContinue();
    void deactivationNext();
    void deactivationReady();

protected:
    virtual void setupModule(); // after xml configuration we can setup and export our module
    void unsetModule();
    virtual void startMeas() =  0;
    virtual void stopMeas() = 0;

    QStateMachine m_ActivationMachine; // we use statemachine for module activation
    QStateMachine m_DeactivationMachine; // and deactivation

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
    qint32 m_nActivationIt;

    std::shared_ptr<BaseModuleConfiguration> m_pConfiguration; // our xml configuration
    QString m_sModuleDescription;

    VfModuleMetaData *m_pModuleName;
    VfModuleMetaData *m_pModuleDescription;
    VfModuleComponent *m_pModuleInterfaceComponent; // here we export the modules interface as json file
    VfModuleComponent *m_pModuleEntityName;

    VeinEvent::EventSystem *m_pModuleEventSystem;

protected slots:
    void activationStart();
    void activationExec();
    void activationDone(); // loop to exec or end up on finished
    virtual void activationFinished() = 0;

    virtual void deactivationStart(); // there are custom overrides around
    void deactivationExec();
    void deactivationDone();
    void deactivationFinished();
private:
    virtual TaskTemplatePtr getModuleSetUpTask(); // Default implementation noop / TODO add activist tasks
    // As log as there are state machines in modules
    bool requiresStateMachineStart() { return true; }
    void startSetupTask();

    const ModuleFactoryParam m_moduleParam;
    TaskTemplatePtr m_startupTask;
    bool m_moduleIsFullySetUp = false;
};

#endif // BASEMODULE_H
