#ifndef BASEMODULE_H
#define BASEMODULE_H

#include "moduleactivist.h"
#include "basemoduleconfiguration.h"
#include "vfmodulemetadata.h"
#include <vfmoduleactvalue.h>
#include <vfmoduleparameter.h>
#include <virtualmodule.h>
#include <xmlsettings.h>
#include <ve_eventsystem.h>
#include <ve_storagesystem.h>

enum LastState
{
    IDLE, CONF, RUN, STOP
};

enum Status
{
    untouched,  // we just got new configuration data that is not yet analyzed
    configured, // we have analyzed config data and config data was ok
    setup,      // we have setup our interface
    activated   // we have activated the module
};


class cBaseModule : public ZeraModules::VirtualModule
{
Q_OBJECT

public:
    cBaseModule(quint8 modnr, int entityId, VeinEvent::StorageSystem* storagesystem, std::shared_ptr<cBaseModuleConfiguration> modcfg, QObject *parent = nullptr);
    virtual ~cBaseModule();
    virtual QList<const QState*> getActualStates() const; // in case parallel working states
    virtual void setConfiguration(QByteArray xmlConfigData);
    virtual QString getModuleName();
    virtual QString getSCPIModuleName();
    virtual quint16 getModuleNr();
    virtual QByteArray getConfiguration() const = 0;
    virtual bool isConfigured() const;
    virtual void startModule();
    virtual void stopModule();
    virtual void exportMetaData();

    int m_nEntityId;
    VeinEvent::StorageSystem* m_pStorageSystem;

    QList<VfModuleMetaData*> veinModuleMetaDataList; // only meta information
    QList<VfModuleComponent*> veinModuleComponentList; // for components that need no scpi interface
    QList<VfModuleActvalue*> veinModuleActvalueList; // actvalues are components that need an interface
    QHash<QString, VfModuleParameter*> veinModuleParameterHash; // parameters are components that need an interface and validation
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
    // additional states for IDLE
    QState* m_pStateIDLEIdle;
    QState* m_pStateIDLEConfXML;
    QState* m_pStateIDLEConfSetup;

    // additional states for RUN
    QState* m_pStateRUNStart;
    QState* m_pStateRUNDone;
    QState* m_pStateRUNDeactivate;
    QState* m_pStateRUNUnset;
    QState* m_pStateRUNConfXML;
    QState* m_pStateRUNConfSetup;

    // additional states for STOP
    QState* m_pStateSTOPStart;
    QState* m_pStateSTOPDone;
    QState* m_pStateSTOPDeactivate;
    QState* m_pStateSTOPUnset;
    QState* m_pStateSTOPConfXML;
    QState* m_pStateSTOPConfSetup;

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


    QByteArray m_xmlconfString;
    QList<cModuleActivist*> m_ModuleActivistList;
    std::shared_ptr<cBaseModuleConfiguration> m_pConfiguration; // our xml configuration
    QString m_sModuleName;
    QString m_sModuleDescription;
    QString m_sSCPIModuleName;
    quint8 m_nModuleNr;

    VfModuleMetaData *m_pModuleName;
    VfModuleMetaData *m_pModuleDescription;
    VfModuleErrorComponent *m_pModuleErrorComponent=nullptr; // here we export errors the module encountered
    VfModuleComponent *m_pModuleInterfaceComponent; // here we export the modules interface as json file
    VfModuleComponent *m_pModuleEntityName;

    virtual void doConfiguration(QByteArray xmlString) = 0; // here we have to do our configuration
    virtual void setupModule(); // after xml configuration we can setup and export our module
    virtual void unsetModule(); // in case of reconfiguration we must unset module first
    virtual void startMeas() =  0;
    virtual void stopMeas() = 0;

    VeinEvent::EventSystem *m_pModuleEventSystem;

protected slots:
    virtual void activationStart() = 0;
    virtual void activationExec() = 0;
    virtual void activationDone() = 0;
    virtual void activationFinished() = 0;

    virtual void deactivationStart() = 0;
    virtual void deactivationExec() = 0;
    virtual void deactivationDone() = 0;
    virtual void deactivationFinished() = 0;
private:
    bool m_bConfCmd;
    bool m_bStartCmd;
    bool m_bStopCmd;
    bool m_bStateMachineStarted;

    QList<const QState*> m_StateList;
    int m_nLastState;
    int m_nStatus;
private slots:
    void entryIdle();
    void entryIDLEIdle();
    void exitIdle();

    void entryConfXML();
    void entryConfSetup();

    void entryRunStart();
    void entryRunDone();
    void entryRunDeactivate();
    void entryRunUnset();
    void entryStopStart();
    void entryStopDone();
};

#endif // BASEMODULE_H