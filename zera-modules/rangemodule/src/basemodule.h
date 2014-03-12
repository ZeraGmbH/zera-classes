#ifndef BASEMODULE_H
#define BASEMODULE_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include <virtualmodule.h>

#include "xmlsettings.h"
#include "moduleacitvist.h"


namespace BaseModule
{
    enum LastState
    {
        IDLE, CONF, RUN, STOP
    };

    enum Status
    {
        untouched, configured, setup, initialized
    };
}


class cBaseMeasProgram;
class cBaseModuleConfiguration;
class cBaseMeasChannel;
class VeinPeer;

class cBaseModule : public ZeraModules::VirtualModule
{
Q_OBJECT

public:
    cBaseModule(Zera::Proxy::cProxy* proxy, VeinPeer* peer, QObject *parent = 0);
    virtual ~cBaseModule();
    virtual QList<const QState*> getActualStates(); // in case parallel working states
    virtual void setConfiguration(QByteArray xmlConfigData);
    virtual QByteArray getConfiguration()=0;
    virtual bool isConfigured();
    virtual void startModule();
    virtual void stopModule();

protected:
    QState* m_pStateConfXML;
    QState* m_pStateConfSetup;
    QState* m_pStateRunStart;
    QState* m_pStateRunDone;
    QState* m_pStateStopStart;
    QState* m_pStateStopDone;

    QByteArray m_xmlconfString;
    Zera::Proxy::cProxy* m_pProxy; // the proxi for all our connections (to rm, dsp- pcb- server)
    VeinPeer* m_pPeer;
    QList<cModuleActivist*> m_ModuleActivistList;

    virtual void doConfiguration(QByteArray xmlString) = 0; // here we have to do our configuration
    virtual void setupModule() = 0; // after xml configuration we can setup and export our module
    virtual void doInitialization() = 0; // here we build our dsp program and start it


private:
    cBaseModuleConfiguration *m_pConfiguration; // our xml configuration
    QList<const QState*> m_StateList;
    int m_nLastState;
    int m_nStatus;

signals:
    void sigRun();
    void sigRunFailed();
    void sigStop();
    void sigStopFailed();
    void sigConfiguration();
    void sigConfDoneIdle();
    void sigConfDoneStop();
    void sigConfDoneRun();
    void sigInitDone();

private slots:
    void entryIdle();
    void exitIdle();
    void entryConf();
    void entryConfXML();
    void entryConfSetup();
    void exitConf();
    void entryRunStart();
    void entryRunDone();
    void entryStopStart();
    void entryStopDone();
};

#endif // RANGEMODULE_H
