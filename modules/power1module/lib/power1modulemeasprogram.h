#ifndef POWERMODULEMEASPROGRAM_H
#define POWERMODULEMEASPROGRAM_H

#include "basedspmeasprogram.h"
#include "power1moduleconfigdata.h"
#include "power1dspvargenerator.h"
#include "dspchainidgen.h"
#include <stringvalidator.h>
#include <measchannelinfo.h>
#include <measmodeselector.h>
#include <meassytemchannels.h>
#include <foutinfo.h>
#include <movingwindowfilter.h>
#include <rminterface.h>
#include <dspinterface.h>
#include <pcbinterface.h>
#include <QFinalState>

namespace POWER1MODULE
{

enum power1moduleCmds
{
    sendrmident,

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

#define irqNr 5

class cPower1Module;

class cPower1ModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cPower1ModuleMeasProgram(cPower1Module* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override; // difference between start and stop is that actual values
    void stop() override; // in interface are not updated when stop
signals:
    void activationSkip();
    void deactivationSkip();
private:
    cPower1ModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();

    QString dspGetPhaseVarStr(int phase, QString separator);
    void dspSetParamsTiMModePhase(int tiTimeOrPeriods);
    QString dspGetSetPhasesVar();

    void setActualValuesNames();
    void setSCPIMeasInfo();
    void setFoutMetaInfo();
    void setModeTypesComponent();

    QString getInitialPhaseOnOffVeinVal();
    MeasSystemChannels getMeasChannelUIPairs();
    void handleMModeParamChange();
    void handleMovingWindowIntTimeChange();
    void updatesForMModeChange();
    double calcTiTime();
    void setPhaseMaskValidator(std::shared_ptr<MeasMode> mode);
    void updatePhaseMaskVeinComponents(std::shared_ptr<MeasMode> mode);
    bool canChangePhaseMask(std::shared_ptr<MeasMode> mode);
    QString getPhasePowerDescription(int measSystemNo);
    struct RangeMaxVals
    {
        double maxU = 0.0;
        double maxI = 0.0;
    };
    RangeMaxVals calcMaxRangeValues(std::shared_ptr<MeasMode> mode);
    QStringList setupMeasModes(DspChainIdGen &dspChainGen);

    void setNominalPowerForQref();
    void generateVeinInterfaceForQrefFreq();
    void generateVeinInterfaceNominalFreq();

    cPower1Module* m_pModule;
    Zera::cRMInterface m_rmInterface;
    Zera::ProxyClientPtr m_rmClient;
    MeasModeSelector m_measModeSelector;
    QHash<QString, cMeasChannelInfo> m_measChannelInfoHash;
    QMap<QString, cFoutInfo> m_FoutInfoMap; // a list with frequency output information for each channel
    QHash<int, QString> m_NotifierInfoHash; // a list with channel information for each notifier

    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    QList<VfModuleParameter*> m_FoutConstParameterList; // a list of foutconstant parameter
    VfModuleMetaData* m_pPQSCountInfo; // the number of values we produce
    VfModuleMetaData* m_pFoutCount; // number of our frequence outputs
    VfModuleMetaData* m_pNomFrequencyInfo; // the modules nominal frequency
    VfModuleParameter* m_pIntegrationParameter;
    VfModuleParameter* m_pMeasuringmodeParameter;
    VfModuleParameter* m_pMModePhaseSelectParameter;
    VfModuleParameter* m_QREFFrequencyParameter = nullptr;
    VfModuleParameter* m_pNominalFrequency = nullptr;
    cStringValidator * m_MModePhaseSelectValidator;
    VfModuleComponent *m_MModeCanChangePhaseMask;
    VfModuleComponent *m_MModePowerDisplayName;
    VfModuleComponent *m_MModeMaxMeasSysCount;
    VfModuleParameter* m_pConstantParameter;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleComponent *m_MModesTypes;

    QList<QPair<VeinStorage::AbstractComponentPtr, VeinStorage::AbstractComponentPtr>> m_scalingInputs;

    QList<QString> infoReadList; // a list of all channel info we have to read
    QString infoRead; // the actual channel info we are working on
    QList<QString> readUrvalueList; // a list with system channel names we need urvalue from
    QString readUrvalueInfo;

    quint8 m_notifierNr;

    Power1DspVarGenerator m_dspVars;

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
    QFinalState m_foutParamsToDsp;

    cMovingwindowFilter m_movingwindowFilter;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void setInterfaceActualValues(QVector<float> *actualValues);

    void resourceManagerConnect();
    void sendRMIdent();

    void claimResourcesSource();
    void claimResourceSource();
    void claimResourceSourceDone();

    void pcbserverConnect4measChannels();
    void pcbserverConnect4freqChannels();

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
    void newPhaseList(QVariant phaseList);
    void newQRefFrequency(QVariant frequency);
    void newNominalFrequency(QVariant frequency);

    void updatePreScaling();
    void onModeTransactionOk();
};

}
#endif // POWERMODULEMEASPROGRAM_H
