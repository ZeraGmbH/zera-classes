#ifndef STATUSMODULEINIT_H
#define STATUSMODULEINIT_H

#include "statusmoduleconfigdata.h"
#include <moduleactivist.h>
#include <proxy.h>
#include <veinmoduleparameter.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <rminterface.h>
#include <QFinalState>

namespace STATUSMODULE
{

namespace STATUSMODINIT
{

enum statusmoduleinitCmds
{
    sendrmident,
    readPCBServerVersion,
    readPCBServerCtrlVersion,
    readPCBServerFPGAVersion,
    readPCBServerSerialNumber,
    readPCBServerAdjStatus,
    readPCBServerAdjChksum,
    registernotifier,
    unregisternotifiers,
    readDSPServerVersion,
    readDSPServerDSPProgramVersion,
    writePCBServerSerialNumber
};

}

class cStatusModule;

class cStatusModuleInit: public cModuleActivist
{
    Q_OBJECT
public:
    cStatusModuleInit(cStatusModule* module, Zera::Proxy::cProxy* proxy, cStatusModuleConfigData& configData);
    virtual ~cStatusModuleInit();
    virtual void generateInterface(); // here we export our interface (entities)
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cStatusModule* m_pModule; // the module we live in
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    cStatusModuleConfigData& m_ConfigData;

    QHash<quint32, int> m_MsgNrCmdList;

    Zera::Server::cPCBInterface *m_pPCBInterface;
    Zera::Proxy::cProxyClient *m_pPCBClient;
    Zera::Server::cDSPInterface *m_pDSPInterface;
    Zera::Proxy::cProxyClient *m_pDSPClient;
    Zera::Server::cRMInterface* m_pRMInterface;
    Zera::Proxy::cProxyClient* m_pRMClient;

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_pcbserverConnectionState; // we try to get a connection to our pcb server
    QState m_pcbserverReadVersionState;
    QState m_pcbserverReadCtrlVersionState;
    QState m_pcbserverReadFPGAVersionState;
    QState m_pcbserverReadSerialNrState;
    QState m_pcbserverReadAdjStatusState;
    QState m_pcbserverReadAdjChksumState;
    QState m_pcbserverRegisterClampCatalogNotifierState;
    QState m_dspserverConnectionState;
    QState m_dspserverReadVersionState;
    QState m_dspserverReadDSPProgramState;
    QFinalState m_activationDoneState; // here we still read the release version

    // statemachine for adjustment-status // -checksum re-read
    QStateMachine m_stateMachineAdjustmentReRead;
    QState m_pcbserverReReadAdjStatusState;
    QState m_pcbserverReReadAdjChksumState;
    QFinalState m_pcbserverReReadDoneState;

    // statemachine for deactivating
    QState m_pcbserverUnregisterClampCatalogNotifierState;
    QFinalState m_deactivationDoneState;

    QString m_sPCBServerVersion;
    QString m_sCtrlVersion;
    QString m_sFPGAVersion;
    QString m_sSerialNumber;
    QString m_sDSPServerVersion;
    QString m_sDSPProgramVersion;
    QString m_sReleaseNumber;
    QString m_sAdjStatus;
    QString m_sAdjChksum;
    QString m_sDeviceType;
    QString m_sCPUInfo;

    cVeinModuleParameter *m_pPCBServerVersion;
    cVeinModuleParameter *m_pCtrlVersion;
    cVeinModuleParameter *m_pFPGAVersion;
    cVeinModuleParameter *m_pSerialNumber;
    cVeinModuleParameter *m_pDSPServerVersion;
    cVeinModuleParameter *m_pDSPProgramVersion;
    cVeinModuleParameter *m_pReleaseNumber;
    cVeinModuleParameter *m_pDeviceType;
    cVeinModuleParameter *m_pCPUInfo;
    cVeinModuleParameter *m_pAdjustmentStatus;
    cVeinModuleParameter *m_pAdjustmentChksum;

    QString findReleaseNr();
    QString findDeviceType();
    QString findCpuInfo();
    void setupDemoOperation();

    QVariant wantedSerialNr;

private slots:
    void setInterfaceComponents();
    void resourceManagerConnect();
    void sendRMIdent();
    void pcbserverConnect();
    void pcbserverReadVersion();
    void pcbserverReadCtrlVersion();
    void pcbserverReadFPGAVersion();
    void pcbserverReadSerialNr();
    void pcbserverReadAdjStatus();
    void pcbserverReadAdjChksum();
    void registerClampCatalogNotifier();
    void unregisterClampCatalogNotifier();
    void dspserverConnect();
    void dspserverReadVersion();
    void dspserverReadDSPProgramVersion();

    void activationDone();
    void deactivationDone();

    void newSerialNumber(QVariant serialNr);

};

}

#endif // STATUSMODULEINIT_H
