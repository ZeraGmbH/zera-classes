#ifndef STATUSMODULEINIT_H
#define STATUSMODULEINIT_H

#include "vfmodulerpc.h"
#include <moduleactivist.h>
#include <vfmoduleparameter.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <timerperiodicqt.h>
#include <QFinalState>
#include <QVariantMap>

namespace STATUSMODULE
{

class cStatusModule;

class cStatusModuleInit: public cModuleActivist
{
    Q_OBJECT
public:
    explicit cStatusModuleInit(cStatusModule* module);
    void generateVeinInterface() override;
signals:
    void activationError();

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer);
    void setInterfaceComponents();
    void pcbserverConnect();
    void pcbserverReadVersion();
    void pcbReadVersion();
    void pcbReadChannelsConnected();
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
    void registerAdjustmentStatusNotifier();
    void getSchnubbelStatus();
    void getAccumulatorStatus();
    void getAccuStateOfCharge();
    void readInstrumentConnected(const QVariant &value);

    void activationDone();
    void deactivationDone();

    void newSerialNumber(const QVariant &serialNr);

    void onVeinUpdate();
private:
    void notifyActivationError(const QVariant &value);
    void createVersionsJson();
    QJsonObject QStringToQJsonObject(const QString &strJson);
    std::shared_ptr<QJsonObject> m_versionsJson;

    cStatusModule* m_pModule = nullptr; // the module we live in

    QHash<quint32, int> m_MsgNrCmdList;

    Zera::PcbInterfacePtr m_pPCBInterface;
    Zera::ProxyClientPtr m_pPCBClient;
    Zera::DspInterfacePtr m_dspInterface;
    Zera::ProxyClientPtr m_dspClient;

    // statemachine for activating gets the following states
    QState m_pcbserverConnectionState; // we try to get a connection to our pcb server
    QState m_pcbserverReadVersionState;
    QState m_pcbReadVersionState;
    QState m_pcbRegisterReadVersionNotifierState;
    QState m_pcbserverReadCtrlVersionState;
    QState m_pcbserverReadFPGAVersionState;
    QState m_pcbserverReadSerialNrState;
    QState m_pcbserverRegisterAdjStatus;
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
    QState m_pcbserverReadChannelsConnectedChange;
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

    VfModuleParameter *m_pPCBServerVersion = nullptr;
    VfModuleParameter *m_pPCBVersion = nullptr;
    VfModuleParameter *m_pCtrlVersion = nullptr;
    VfModuleParameter *m_pFPGAVersion = nullptr;
    VfModuleParameter *m_pSerialNumber = nullptr;
    VfModuleParameter *m_pDSPServerVersion = nullptr;
    VfModuleParameter *m_pDSPProgramVersion = nullptr;
    VfModuleParameter *m_pReleaseNumber = nullptr;
    VfModuleParameter *m_pDeviceType = nullptr;
    VfModuleParameter *m_pCPUInfo = nullptr;
    VfModuleParameter *m_pAdjustmentStatus = nullptr;
    VfModuleParameter *m_pAdjustmentChksum = nullptr;
    VfModuleParameter *m_pSchnubbelStatus = nullptr;
    VfModuleParameter *m_pAccumulatorStatus = nullptr;
    VfModuleParameter *m_pAccumulatorSoc = nullptr;
    VfModuleParameter *m_pInstrument = nullptr;
    VfModuleParameter *m_pChannels = nullptr;
    VfModuleRpcPtr m_createVersionFileRpc = nullptr;
    VfModuleRpcPtr m_resetAdjDataRpc;

    QVariant wantedSerialNr;

    VfModuleParameter* m_veinDspBusy = nullptr;
    VfModuleParameter* m_veinDspPeriodCount = nullptr;
    VfModuleParameter* m_veinDspMsTimer = nullptr;
    TimerTemplateQtPtr m_veinUpdateTimer;
    quint32 m_lastVeinPeriod = 0;
    quint32 m_lastVeinTimeMs = 0;
};

}

#endif // STATUSMODULEINIT_H
