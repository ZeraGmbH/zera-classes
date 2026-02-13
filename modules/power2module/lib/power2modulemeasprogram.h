#ifndef POWER2MODULEMEASPROGRAM_H
#define POWER2MODULEMEASPROGRAM_H

#include "power2module.h"
#include "power2moduleconfigdata.h"
#include "measchannelinfo.h"
#include "foutinfo.h"
#include <measmodeselector.h>
#include <basedspmeasprogram.h>
#include <movingwindowfilter.h>
#include <timerperiodicqt.h>

namespace POWER2MODULE
{

enum power2moduleCmds
{
    sendrmident,
    readresourcetypes,

    claimresourcesource,

    readsourcechannelalias,
    readsourcechanneldspchannel,
    readsourceformfactor,

    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
    freeresourcesource,
    unregisterrangenotifiers,
    readurvalue,
    setchannelrangenotifier,
    setqrefnominalpower
};

#define irqNr 7

class cPower2ModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cPower2ModuleMeasProgram(cPower2Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    virtual void start() override;
    virtual void stop() override;
signals:
    void activationSkip();
    void deactivationSkip();
private:
    cPower2ModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();

    QStringList dspCmdInitVars(int dspInitialSelectCode);
    void dspSetParamsTiMModePhase(int tiTimeOrPeriods);

    void setActualValuesNames();

    void handleMovingWindowIntTimeChange();
    void handleMModeParamChange();
    void updatesForMModeChange();
    double calcTiTime();
    void setPhaseMaskValidator(std::shared_ptr<MeasMode> mode);
    void updatePhaseMaskVeinComponents(std::shared_ptr<MeasMode> mode);
    struct RangeMaxVals
    {
        double maxU = 0.0;
        double maxI = 0.0;
    };
    RangeMaxVals calcMaxRangeValues(std::shared_ptr<MeasMode> mode);
    quint8 cmpActualValIndex(freqoutconfiguration frconf);

    cPower2Module* m_pModule;
    Zera::cRMInterface m_rmInterface;
    Zera::ProxyClientPtr m_rmClient;
    MeasModeSelector m_measModeSelector;
    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QMap<QString, cFoutInfo> m_FoutInfoMap; // a list with frequency output information for each channel
    QHash<int, QString> m_NotifierInfoHash; // a list with channel information for each notifier

    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pPQSCountInfo; // the number of values we produce
    VfModuleMetaData* m_pFoutCount; // number of our frequence outputs
    VfModuleMetaData* m_pNomFrequencyInfo; // the modules nominal frequency
    VfModuleParameter* m_pIntegrationParameter;
    VfModuleParameter* m_pMeasuringmodeParameter;
    VfModuleComponent* m_pMeasureSignal;

    QList<QString> infoReadList; // a list of all channel info we have to read
    QString infoRead; // the actual channel info we are working on
    QList<QString> readUrvalueList; // a list with system channel names we need urvalue from
    QString readUrvalueInfo;

    quint8 m_notifierNr;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;
    cDspMeasData* m_pfreqScaleDSP;
    cDspMeasData* m_pNomPower;

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;

    QState m_claimResourcesSourceState;
    QState m_claimResourceSourceState;
    QState m_claimResourceSourceDoneState;

    QState m_pcbserverConnectState4measChannels;
    QState m_pcbserverConnectState4freqChannels;

    QState m_readSourceChannelInformationState;
    QState m_readSourceChannelAliasState;
    QState m_readSourceDspChannelState;
    QState m_readSourceFormFactorState;
    QState m_readSourceChannelInformationDoneState;

    QState m_setSenseChannelRangeNotifiersState;
    QState m_setSenseChannelRangeNotifierState;
    QState m_setSenseChannelRangeNotifierDoneState;

    QState m_dspserverConnectState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_freeFreqOutputsState;
    QState m_freeFreqOutputState;
    QState m_freeFreqOutputDoneState;

    QState m_resetNotifiersState;
    QState m_resetNotifierState;
    QState m_resetNotifierDoneState;

    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    QStateMachine m_readUpperRangeValueMachine;
    QState m_readUrvalueState;
    QState m_readUrvalueDoneState;
    QState m_foutParamsToDsp;
    QFinalState m_setFoutConstantState;

    cMovingwindowFilter m_movingwindowFilter;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void setSCPIMeasInfo();
    void setInterfaceActualValues(QVector<float> *actualValues);

    void resourceManagerConnect();
    void sendRMIdent();

    void claimResourcesSource();
    void claimResourceSource();
    void claimResourceSourceDone();

    void pcbserverConnect4measChannels();
    void pcbserverConnect4freqChannels();
    void readSenseChannelInformation();
    void readSenseChannelInformationDone();

    void readSourceChannelInformation();
    void readSourceChannelAlias();
    void readSourceDspChannel();
    void readSourceFormFactor();
    void readSourceChannelInformationDone();

    void setSenseChannelRangeNotifiers();
    void setSenseChannelRangeNotifier();
    void setSenseChannelRangeNotifierDone();

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void freeFreqOutputs();
    void freeFreqOutput();
    void freeFreqOutputDone();
    void resetNotifiers();
    void resetNotifier();
    void resetNotifierDone();

    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void readUrvalue();
    void readUrvalueDone();
    void foutParamsToDsp();
    void setFoutPowerModes();

    void newIntegrationtime(QVariant ti);
    void newIntegrationPeriod(QVariant period);
    void newMeasMode(QVariant mm);

    void onModeTransactionOk();
};

}
#endif // POWER2MODULEMEASPROGRAM_H
