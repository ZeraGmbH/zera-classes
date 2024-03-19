#ifndef RMSMODULEMEASPROGRAM_H
#define RMSMODULEMEASPROGRAM_H

#include "rmsmoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <movingwindowfilterwithoutsumfifo.h>
#include <timerperiodicqt.h>
#include "actualvaluestartstophandler.h"
#include <QFinalState>

namespace RMSMODULE
{

enum rmsmoduleCmds
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

class cRmsModule;

class cRmsModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cRmsModuleMeasProgram(cRmsModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual void generateInterface() override; // here we export our interface (entities)
public slots:
    virtual void start() override; // difference between start and stop is that actual values
    virtual void stop() override; // in interface are not updated when stop
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cRmsModuleConfigData* getConfData();
    void setDspVarList();
    void deleteDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();

    cRmsModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QList<QString> channelInfoReadList; // a list of all channel info we have to read
    QString channelInfoRead; // the actual channel info we are working on
    quint32 m_nSRate; // number of samples / signal period

    VfModuleComponent *m_pMeasureSignal;
    VfModuleParameter* m_pIntegrationParameter;
    VfModuleMetaData* m_pRMSPNCountInfo;
    VfModuleMetaData* m_pRMSPPCountInfo;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
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

    MovingWindowFilterWithoutSumFifo m_movingwindowFilter;

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

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
    void newIntegrationPeriod(QVariant period);

};

}
#endif // RMSMODULEMEASPROGRAM_H
