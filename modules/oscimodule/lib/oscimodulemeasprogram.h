#ifndef OSCIMODULEMEASPROGRAM_H
#define OSCIMODULEMEASPROGRAM_H

#include "actualvaluestartstophandler.h"
#include "oscimoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <QFinalState>
#include <timerperiodicqt.h>

namespace OSCIMODULE
{

enum oscimoduleCmds
{
    sendrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    readalias,
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

#define irqNr 6

class cOsciModule;

class cOsciModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cOsciModuleMeasProgram(cOsciModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cOsciModuleConfigData* getConfData();
    void setDspVarList();
    void deleteDspVarList();
    void setDspCmdList();
    void deleteDspCmdList();

    cOsciModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleActvalue*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pOsciCountInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pRefChannelParameter;

    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QList<QString> channelInfoReadList; // a list of all channel info we have to read
    QString channelInfoRead; // the actual channel info we are working on

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
    QState m_readChannelInformationState;
    QState m_readChannelAliasState;
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
    void readChannelInformation();
    void readChannelAlias();
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

    void newRefChannel(QVariant chn);

};

}
#endif // OSCIMODULEMEASPROGRAM_H
