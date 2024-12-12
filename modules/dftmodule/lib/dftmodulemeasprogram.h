#ifndef DFTMODULEMEASPROGRAM_H
#define DFTMODULEMEASPROGRAM_H

#include "dftmodule.h"
#include "dftmoduleconfiguration.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <movingwindowfilter.h>
#include <timerperiodicqt.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace DFTMODULE
{

enum dftmoduleCmds
{
    sendrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    readsamplerate,
    readalias,
    readunit,
    readdspchannel,
    claimpgrmem,
    claimusermem,
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
    freepgrmem,
    freeusermem
};

class cDftModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cDftModuleMeasProgram(cDftModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;

public slots:
    virtual void start() override; // difference between start and stop is that actual values
    virtual void stop() override; // in interface are not updated when stop
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cDftModuleConfigData* getConfData();
    void setDspVarList();
    void deleteDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();
    void turnVectorsToRefChannel();

    cDftModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    QList<VfModuleActvalue*> m_veinPolarValue;
    VfModuleActvalue* m_pRFieldActualValue;
    QMap<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QHash<QString, QString> m_ChannelSystemNameHash; // a hash for fast access to the system name with key = alias
    QList<QString> channelInfoReadList; // a list of all channel info we have to read
    QList<int> rfieldActvalueIndexList; // an index list for access to configured actual values
    QString channelInfoRead; // the actual channel info we are working on
    quint32 m_nSRate; // number of samples / signal period

    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pIntegrationTimeParameter;
    VfModuleParameter* m_pRefChannelParameter;
    VfModuleMetaData* m_pDFTPNCountInfo;
    VfModuleMetaData* m_pDFTPPCountInfo;
    VfModuleMetaData* m_pDFTOrderInfo;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState m_channelRangeObserverScanState;
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_readResourceTypesState;
    QState m_readResourceState;
    QState m_readResourceInfosState;
    QState m_readResourceInfoState;
    QState m_readResourceInfoDoneState;
    QState m_pcbserverConnectState;
    QState m_readSampleRateState;
    QState m_readChannelInformationState;
    QState m_readChannelAliasState;
    QState m_readChannelUnitState;
    QState m_readDspChannelState;
    QState m_readDspChannelDoneState;

    QState m_dspserverConnectState;
    QState m_claimPGRMemState;
    QState m_claimUSERMemState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_deactivateDSPState;
    QState m_freePGRMemState;
    QState m_freeUSERMemState;
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    void setActualValuesNames();
    void setSCPIMeasInfo();
    void setRefChannelValidator();
    void initRFieldMeasurement();

    cMovingwindowFilter m_movingwindowFilter;

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void startFetchCommonRanges();
    void resourceManagerConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfos();
    void readResourceInfo();
    void readResourceInfoDone();

    void pcbserverConnect();
    void readSampleRate();
    void readChannelInformation();
    void readChannelAlias();
    void readChannelUnit();
    void readDspChannel();
    void readDspChannelDone();

    void dspserverConnect();
    void claimPGRMem();
    void claimUSERMem();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSP();
    void freePGRMem();
    void freeUSERMem();
    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void newIntegrationtime(QVariant ti);
    void newRefChannel(QVariant refchn);
};

}
#endif // DFTMODULEMEASPROGRAM_H
