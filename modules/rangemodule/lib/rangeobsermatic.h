#ifndef RANGEOBSERMATIC_H
#define RANGEOBSERMATIC_H

#include "rangemoduleconfigdata.h"
#include "rangemeaschannel.h"
#include <moduleactivist.h>
#include "abstractfactoryserviceinterfaces.h"
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
    readgain2corr,
    writegain2corr,
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
    cRangeObsermatic(cRangeModule* module, QList<QStringList> groupList, QStringList chnlist, cObsermaticConfPar& confpar);
    virtual void generateInterface(); // here we export our interface (entities)
    VfModuleComponent *m_pRangingSignal;
public slots:
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);
    void catchChannelNewRangeList();
private:
    cRangeModule *m_pModule;
    QList<QStringList> m_GroupList;
    QStringList m_ChannelNameList; // the system names of our channels
    cObsermaticConfPar& m_ConfPar;
    DspInterfacePtr m_dspInterface;
    Zera::ProxyClientPtr m_dspClient;
    QStringList m_ChannelAliasList; // the alias of our channels
    QList<cRangeMeasChannel*> m_RangeMeasChannelList;
    QHash<QString,cStringValidator*> m_ChannelRangeValidatorHash;
    QList<bool> m_softOvlList; // here we enter software detected overloads
    QList<bool> m_hardOvlList; // what do you think ?
    QList<bool> m_groupOvlList; // distinct group overloads: one in a group causes reset for all in the group
    QList<bool> m_maxOvlList; // here we enter overloads that occured in maximum range
    quint32 m_nWaitAfterRanging;
    quint32 m_nRangeSetPending;
    quint32 m_nReadStatusPending;
    QStringList m_actChannelRangeList; // a list of the actual ranges set
    QStringList m_actChannelRangeNotifierList; // we hold a list to avoid unnecessary notifications

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

    cDspMeasData* m_pGainCorrection2DSP; // copy of dsp internal correction data
    float* m_pfScale;
    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for activation (reading dsp correction data used for scaling)
    QState m_dspserverConnectState;
    QState m_readGainCorrState;
    QFinalState m_readGainCorrDoneState;

    // statemachine for deactivating
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;

    // statemachine for writing dsp correction data used for scaling
    QStateMachine m_writeCorrectionDSPMachine;
    QState m_writeGainCorrState;
    QState m_writeGainCorrRepeatState;
    QFinalState m_writeGainCorrDoneState;

    bool m_brangeSet; // we set this here after we selected a new range and enable resetting stored overloadcondition

    void rangeObservation();
    void rangeAutomatic();
    void groupHandling();
    void setRanges(bool force = false); // here we really set ranges
    QList<int> getGroupIndexList(int index);
    bool requiresOverloadReset(int channel);
    void startOverloadReset(int channel);
    /**
     * @brief getPreScale
     *
     * Reads prescaling factor for group with p_id
     * p_idx is equal channel position in m_RangeMeasChannelList
     *
     * @param p_idx: channel id
     * @return scalingfactor
     */
    float getPreScale(int p_idx);

private slots:
    void dspserverConnect();
    void readGainCorr();
    void readGainCorrDone();

    void deactivationInit();
    void deactivationDone();

    void writeGainCorr();
    void writeGainCorrDone();
    void readStatus();
    void analyzeStatus();

    void newRange(QVariant range);
    void newRangeAuto(QVariant rauto);
    void newGrouping(QVariant rgrouping);
    void newOverload(QVariant overload);
    void preScalingChanged(QVariant unused);

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant);
};

}

#endif // RANGEOBSERMATIC_H
