#ifndef POWERMODULEMEASPROGRAM_H
#define POWERMODULEMEASPROGRAM_H

#include "power1moduleconfigdata.h"
#include <stringvalidator.h>
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <measmodeselector.h>
#include <meassytemchannels.h>
#include <foutinfo.h>
#include <vfmodulecomponentinput.h>
#include <movingwindowfilter.h>
#include <rminterface.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace POWER1MODULE
{

enum power1moduleCmds
{
    resourcemanagerconnect,
    sendrmident,
    readresourcetypes,
    readresourcesense,
    readresourcesenseinfos,
    readresourcesenseinfo,

    readresourcesource,
    readresourcessourceinfos,
    readresourcesourceinfo,
    claimresourcesource,

    pcbserverconnect,
    readsamplerate,

    readsensechannelalias,
    readsensechannelunit,
    readsensechanneldspchannel,

    readsourcechannelalias,
    readsourcechanneldspchannel,
    readsourceformfactor,

    claimpgrmem,
    claimusermem,
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
    freepgrmem,
    freeusermem,
    freeresourcesource,
    unregisterrangenotifiers,
    readurvalue,
    setchannelrangenotifier,
    setqrefnominalpower
};

#define irqNr 5

class cPower1Module;

class cPower1ModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cPower1ModuleMeasProgram(cPower1Module* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    virtual ~cPower1ModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop
protected:
    virtual void setDspVarList(); // dsp related stuff
    virtual void deleteDspVarList();
    virtual void setDspCmdList();
    virtual void deleteDspCmdList();
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    cPower1ModuleConfigData* getConfData();
    void setActualValuesNames();
    void setSCPIMeasInfo();
    void setFoutMetaInfo();

    QString getInitialPhaseOnOffVeinVal();
    MeasSystemChannels getMeasChannelUIPairs();
    QString dspGetPhaseVarStr(int phase, QString separator);
    void dspSetParamsTiMModePhase(int tiTimeOrPeriods);
    QString dspGetSetPhasesVar();
    void handleMModeParamChange();
    void handleMovingWindowIntTimeChange();
    void updatesForMModeChange();
    double calcTiTime();
    void setPhaseMaskValidator(std::shared_ptr<MeasMode> mode);
    void updatePhaseMaskVeinComponents(std::shared_ptr<MeasMode> mode);
    bool canChangePhaseMask(std::shared_ptr<MeasMode> mode);
    struct RangeMaxVals
    {
        double maxU = 0.0;
        double maxI = 0.0;
    };
    RangeMaxVals calcMaxRangeValues(std::shared_ptr<MeasMode> mode);

    cPower1Module* m_pModule;
    MeasModeSelector m_measModeSelector;
    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QHash<QString, cFoutInfo> m_FoutInfoHash; // a list with frequency output information for each channel
    QHash<int, QString> m_NotifierInfoHash; // a list with channel information for each notifier

    QList<VfModuleActvalue*> m_ActValueList; // the list of actual values we work on
    QList<VfModuleParameter*> m_FoutConstParameterList; // a list of foutconstant parameter
    VfModuleMetaData* m_pPQSCountInfo; // the number of values we produce
    VfModuleMetaData* m_pFoutCount; // number of our frequence outputs
    VfModuleMetaData* m_pNomFrequencyInfo; // the modules nominal frequency
    VfModuleParameter* m_pIntegrationParameter;
    VfModuleParameter* m_pMeasuringmodeParameter;
    VfModuleParameter* m_pMModePhaseSelectParameter;
    cStringValidator * m_MModePhaseSelectValidator;
    VfModuleActvalue *m_MModeCanChangePhaseMask;
    VfModuleActvalue *m_MModePowerDisplayName;
    VfModuleActvalue *m_MModeMaxMeasSysCount;
    VfModuleParameter* m_pConstantParameter;
    VfModuleComponent* m_pMeasureSignal;

    QList<QPair<VfModuleComponentInput*,VfModuleComponentInput*>> m_pScalingInputs; //scaling component list

    QList<QString> infoReadList; // a list of all channel info we have to read
    QString infoRead; // the actual channel info we are working on
    QList<QString> readUrvalueList; // a list with system channel names we need urvalue from
    QString readUrvalueInfo;

    quint32 m_nSRate; // number of samples / signal period
    quint8 m_notifierNr;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;
    cDspMeasData* m_pfreqScaleDSP;
    cDspMeasData* m_pNomPower;

    // statemachine for activating gets the following states
    QState m_resourceManagerConnectState;
    QState m_IdentifyState;
    QState m_readResourceTypesState;

    QState m_readResourceSenseState;
    QState m_readResourceSenseInfosState;
    QState m_readResourceSenseInfoState;
    QState m_readResourceSenseInfoDoneState;

    QState m_readResourceSourceState;
    QState m_claimResourcesSourceState;
    QState m_claimResourceSourceState;
    QState m_claimResourceSourceDoneState;

    QState m_readResourceSourceInfosState;
    QState m_readResourceSourceInfoState;
    QState m_readResourceSourceInfoDoneState;

    QState m_pcbserverConnectState4measChannels;
    QState m_pcbserverConnectState4freqChannels;
    QState m_readSampleRateState;

    QState m_readSenseChannelInformationState;
    QState m_readSenseChannelAliasState;
    QState m_readSenseChannelUnitState;
    QState m_readSenseDspChannelState;
    QState m_readSenseChannelInformationDoneState;

    QState m_readSourceChannelInformationState;
    QState m_readSourceChannelAliasState;
    QState m_readSourceDspChannelState;
    QState m_readSourceFormFactorState;
    QState m_readSourceChannelInformationDoneState;

    QState m_setSenseChannelRangeNotifiersState;
    QState m_setSenseChannelRangeNotifierState;
    QState m_setSenseChannelRangeNotifierDoneState;

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

    QStateMachine m_readUrValueMachine;
    QState m_readUrvalueState;
    QState m_readUrvalueDoneState;
    QFinalState m_foutParamsToDsp;

    cMovingwindowFilter m_movingwindowFilter;

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void resourceManagerConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResourceSense();
    void readResourceSenseInfos();
    void readResourceSenseInfo();
    void readResourceSenseInfoDone();
    void readResourceSource();
    void readResourceSourceInfos();
    void readResourceSourceInfo();
    void readResourceSourceInfoDone();

    void claimResourcesSource();
    void claimResourceSource();
    void claimResourceSourceDone();

    void pcbserverConnect4measChannels();
    void pcbserverConnect4freqChannels();
    void readSampleRate();
    void readSenseChannelInformation();
    void readSenseChannelAlias();
    void readSenseChannelUnit();
    void readSenseDspChannel();
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
    void claimPGRMem();
    void claimUSERMem();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSP();
    void freePGRMem();
    void freeUSERMem();
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
    void newPhaseList(QVariant phaseList);

    void updatePreScaling(QVariant p_newValue);
    void onModeTransactionOk();
};

}
#endif // POWERMODULEMEASPROGRAM_H
