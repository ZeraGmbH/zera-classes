#ifndef STATUSMODULEINIT_H
#define STATUSMODULEINIT_H

#include "statusmoduleconfigdata.h"
#include <moduleactivist.h>
#include <vfmoduleparameter.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <timerperiodicqt.h>
#include <QFinalState>
#include <QVariantMap>

namespace STATUSMODULE
{

namespace STATUSMODINIT
{

enum statusmoduleinitCmds
{
    readPCBServerVersion,
    readPCBInfo,
    readPCBServerCtrlVersion,
    readPCBServerFPGAVersion,
    readPCBServerSerialNumber,
    readPCBServerAdjStatus,
    readPCBServerAdjChksum,
    registerClampCatalogNotifier,
    unregisterNotifiers,
    readDSPServerVersion,
    readDSPServerDSPProgramVersion,
    writePCBServerSerialNumber,
    registerSchnubbelStatusNotifier,
    readPCBServerSchnubbelStatus,
    registerAccumulatorStatusNotifier,
    readPCBServerAccumulatorStatus,
    registerAccumulatorSocNotifier,
    readPCBServerAccumulatorSoc,
    registerCtrlVersionChange,
    registerPCBVersionChange
};

enum NOTIFIER_IDS
{
    clampNotifierID = 1,
    schnubbelNotifierID,
    accumulatorStatusNotifierID,
    accumulatorSocNotifierID,
    ctrlVersionChangeID,
    pcbVersionChangeID
};

}

class cStatusModule;

class cStatusModuleInit: public cModuleActivist
{
    Q_OBJECT
public:
    cStatusModuleInit(cStatusModule* module, cStatusModuleConfigData& configData);
    virtual ~cStatusModuleInit();
    void generateVeinInterface() override;
signals:
    void activationError();
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void notifyActivationError(QVariant value);

    cStatusModule* m_pModule; // the module we live in
    cStatusModuleConfigData& m_ConfigData;

    QHash<quint32, int> m_MsgNrCmdList;

    Zera::PcbInterfacePtr m_pPCBInterface;
    Zera::ProxyClientPtr m_pPCBClient;
    Zera::cDSPInterface *m_pDSPInterface;
    Zera::ProxyClient *m_pDSPClient;

    // statemachine for activating gets the following states
    QState m_pcbserverConnectionState; // we try to get a connection to our pcb server
    QState m_pcbserverReadVersionState;
    QState m_pcbReadVersionState;
    QState m_pcbRegisterReadVersionNotifierState;
    QState m_pcbserverReadCtrlVersionState;
    QState m_pcbserverReadFPGAVersionState;
    QState m_pcbserverReadSerialNrState;
    QState m_pcbserverReadAdjStatusState;
    QState m_pcbserverReadAdjChksumState;
    QState m_pcbserverRegisterClampCatalogNotifierState;
    QState m_dspserverConnectionState;
    QState m_dspserverReadVersionState;
    QState m_dspserverReadDSPProgramState;
    QState m_pcbserverRegisterSchnubbelStatusNotifierState;
    QState m_pcbserverReadInitialSchnubbelStatus;
    QState m_pcbserverRegisterAccumulatorStatusNotifierState;
    QState m_pcbserverReadInitialAccumulatorStatus;
    QState m_pcbserverRegisterAccumulatorSocNotifierState;
    QState m_pcbserverReadInitialAccumulatorSoc;
    QState m_pcbserverRegisterCtrlVersionChange;
    QFinalState m_activationDoneState; // here we still read the release version

    // statemachine for adjustment-status // -checksum re-read
    QStateMachine m_stateMachineAdjustmentReRead;
    QState m_pcbserverReReadAdjStatusState;
    QState m_pcbserverReReadAdjChksumState;
    QFinalState m_pcbserverReReadDoneState;

    // statemachine for deactivating
    QState m_pcbserverUnregisterNotifiersState;
    QFinalState m_deactivationDoneState;

    QString m_sPCBServerVersion;
    QString m_sPCBVersion;
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

    VfModuleParameter *m_pPCBServerVersion;
    VfModuleParameter *m_pPCBVersion;
    VfModuleParameter *m_pCtrlVersion;
    VfModuleParameter *m_pFPGAVersion;
    VfModuleParameter *m_pSerialNumber;
    VfModuleParameter *m_pDSPServerVersion;
    VfModuleParameter *m_pDSPProgramVersion;
    VfModuleParameter *m_pReleaseNumber;
    VfModuleParameter *m_pDeviceType;
    VfModuleParameter *m_pCPUInfo;
    VfModuleParameter *m_pAdjustmentStatus;
    VfModuleParameter *m_pAdjustmentChksum;
    VfModuleParameter *m_pSchnubbelStatus;
    VfModuleParameter *m_pAccumulatorStatus;
    VfModuleParameter *m_pAccumulatorSoc;

    QVariant wantedSerialNr;

    QString findReleaseNr();
    QString findCpuInfo();

private slots:
    void setInterfaceComponents();
    void pcbserverConnect();
    void pcbserverReadVersion();
    void pcbReadVersion();
    void pcbserverReadCtrlVersion();
    void pcbserverReadFPGAVersion();
    void pcbserverReadSerialNr();
    void pcbserverReadAdjStatus();
    void pcbserverReadAdjChksum();
    void registerClampCatalogNotifier();
    void unregisterNotifiers();
    void dspserverConnect();
    void dspserverReadVersion();
    void dspserverReadDSPProgramVersion();
    void registerSchnubbelStatusNotifier();
    void registerAccumulatorStatusNotifier();
    void registerAccumulatorSocNotifier();
    void registerCtrlVersionsChangedNotifier();
    void registerPCBVersionNotifier();
    void getSchnubbelStatus();
    void getAccumulatorStatus();
    void getAccuStateOfCharge();

    void activationDone();
    void deactivationDone();

    void newSerialNumber(QVariant serialNr);
};

}

#endif // STATUSMODULEINIT_H
