#ifndef RANGEOBSERMATIC_H
#define RANGEOBSERMATIC_H

#include "rangemoduleconfigdata.h"
#include "rangemeaschannel.h"
#include <moduleactivist.h>
#include <dspinterface.h>
#include <vfmoduleparameter.h>
#include <networkconnectioninfo.h>
#include <stringvalidator.h>
#include <dspinterface.h>

// cRangeObsermatic is responsible for setting, observing, automatic setting
// and grouping measurement channel ranges. it works on a flexible list of
// system measurement channels.
// it provides its interface :
// ChannelXRange Input/Output for range setting
// ChannelXOVR   Output information of overload conditions
// RangeAutomatic ON/OFF Input
// GroupHandling ON/OFF Input
// !!! important range obsermatic must be activated after !!! activating rangemeaschannels

namespace RANGEMODULE
{

enum rangeObsermaticCmds
{
    readgainscale,
    writegainScale,
    resetstatus,
    readstatus,

    setrange, // we reserve 32 values for setrange

    nextCmd = setrange+32
};

class cRangeModule;

class cRangeObsermatic: public cModuleActivist
{
    Q_OBJECT
public:
    cRangeObsermatic(cRangeModule* module, QList<QStringList> groupList, cObsermaticConfPar& confpar);
    void generateVeinInterface() override;
    VfModuleComponent *m_pRangingSignal;
signals:
    void activationRepeat();
public slots:
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);
    void catchChannelNewRangeList();
private:
    cRangeModule *m_pModule;
    QList<QStringList> m_GroupList;
    cObsermaticConfPar& m_ConfPar;
    Zera::DspInterfacePtr m_dspInterface;
    Zera::ProxyClientPtr m_dspClient;
    QList<cRangeMeasChannel*> m_RangeMeasChannelList;
    QHash<QString,cStringValidator*> m_ChannelRangeValidatorHash;
    QList<bool> m_softOvlList; // here we enter software detected overloads
    QList<bool> m_hardOvlList; // what do you think ?
    QList<bool> m_groupOvlList; // distinct group overloads: one in a group causes reset for all in the group
    QList<bool> m_maxOvlList; // here we enter overloads that occured in maximum range
    quint32 m_nWaitAfterRanging = 0;
    quint32 m_nRangeSetPending = 0;
    quint32 m_nReadStatusPending = 0;
    QStringList m_actChannelRangeList; // a list of the actual ranges set

    // our interface entities
    QList<VfModuleParameter*> m_RangeParameterList;
    QList<VfModuleComponent*> m_RangeOVLComponentList;
    QList<VfModuleComponent*> m_RangeOVLRejectionComponentList;
    QList<VfModuleComponent*> m_RangeActRejectionComponentList;
    QList<VfModuleComponent*> m_RangeActOVLRejectionComponentList;
    QList<VfModuleParameter*> m_RangeGroupPreScalingList;
    QList<VfModuleParameter*> m_RangeGroupPreScalingEnabledList;
    QList<VfModuleComponent*> m_RangeGroupPreScalingInfo;

    VfModuleParameter* m_pParRangeAutomaticOnOff;
    VfModuleParameter* m_pParGroupingOnOff;
    VfModuleParameter* m_pParOverloadOnOff;
    VfModuleComponent* m_pComponentOverloadMax;

    cDspMeasData* m_gainScaleDSPMemHandle = nullptr; // copy of dsp internal correction data
    cDspVar *m_gainScaleDspVar = nullptr;
    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for activation (reading dsp correction data used for scaling)
    QState m_dspserverConnectState;
    QState m_readGainCorrState;
    QFinalState m_readGainCorrDoneState;

    // statemachine for deactivating
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;

    // statemachine for writing dsp correction data used for scaling
    QStateMachine m_writeDspGainScaleStateMachine;
    QState m_writeGainScaleState;
    QState m_writeGainScaleRepeatState;
    QFinalState m_writeGainScaleDoneState;

    bool m_rangeSetManual = false; // we set this here after we selected a new range and enable resetting stored overloadcondition

    void rangeObservation();
    void rangeAutomatic();
    void groupHandling();
    void setRanges(bool force = false); // here we really set ranges
    QList<int> getGroupAliasIdxListForChannel(int channelIdx);
    bool requiresOverloadReset(int channel);
    void startOverloadReset(int channel);
    double getPreScale(int channelIdx);
    void handleOverload(const int channelIdx, bool rmsOverload, bool hardOverLoad, bool adcOverLoad);
    void setOverloadVeinComponent(bool overloadOn);

private slots:
    void dspserverConnect();
    void readGainScale();
    void readGainScaleDone();

    void deactivationInit();
    void deactivationDone();

    void writeGainScale();
    void readStatus();
    void analyzeStatus();

    void onNewRange(QVariant range);
    void newRangeAuto(QVariant rauto);
    void newGrouping(QVariant rgrouping);
    void onVeinChangeOverload(QVariant overload);
    void preScalingChanged(QVariant unused);

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant);
};

}

#endif // RANGEOBSERMATIC_H
