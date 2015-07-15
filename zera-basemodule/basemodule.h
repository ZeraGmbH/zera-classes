#ifndef BASEMODULE_H
#define BASEMODULE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QList>
#include <QByteArray>
#include <QTimer>
#include <virtualmodule.h>

#include "xmlsettings.h"
#include "moduleactivist.h"


class VeinPeer;
class VeinEntity;

namespace VeinEvent
{
    class EventSystem;
}

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

class cVeinModuleComponent;
class cBaseMeasProgram;
class cBaseModuleConfiguration;
class cBaseMeasChannel;


class cBaseModule : public ZeraModules::VirtualModule
{
Q_OBJECT

public:
    cBaseModule(quint8 modnr, Zera::Proxy::cProxy* proxy, int entityId, VeinEvent::EventSystem* eventsystem, cBaseModuleConfiguration* modcfg, QObject *parent = 0);
    virtual ~cBaseModule();
    virtual QList<const QState*> getActualStates(); // in case parallel working states
    virtual void setConfiguration(QByteArray xmlConfigData);
    virtual QString getModuleName();
    virtual QString getSCPIModuleName();
    virtual quint16 getModuleNr();
    virtual QByteArray getConfiguration()=0;
    virtual bool isConfigured();
    virtual void startModule();
    virtual void stopModule();

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


    QByteArray m_xmlconfString;
    Zera::Proxy::cProxy* m_pProxy; // the proxi for all our connections (to rm, dsp- pcb- server)
    int m_nEntityId;
    VeinEvent::EventSystem* m_pEventsystem;
    QList<cModuleActivist*> m_ModuleActivistList;
    cBaseModuleConfiguration *m_pConfiguration; // our xml configuration
    QString m_sModuleName;
    QString m_sSCPIModuleName;
    quint8 m_nModuleNr;
    cVeinModuleComponent *m_pModuleErrorComponent;
    cVeinModuleComponent *m_pModuleInterfaceComponent; // here we export the modules interface as json file

    virtual void doConfiguration(QByteArray xmlString) = 0; // here we have to do our configuration
    virtual void setupModule() = 0; // after xml configuration we can setup and export our module
    virtual void unsetModule() = 0; // in case of reconfiguration we must unset module first
    virtual void startMeas() =  0;
    virtual void stopMeas() = 0;

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
