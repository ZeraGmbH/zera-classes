#ifndef RANGEOBSERMATIC_H
#define RANGEOBSERMATIC_H

#include "rangemoduleconfigdata.h"
#include "rangemeaschannel.h"
#include <moduleactivist.h>
#include <veinmoduleparameter.h>
#include <socket.h>
#include <proxy.h>
#include <stringvalidator.h>
#include <service-interfaces/dspinterface.h>
#include <QTimer>

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
    cRangeObsermatic(cRangeModule* module, Zera::Proxy::cProxy* proxy, cSocket* dspsocket, QList<QStringList> groupList, QStringList chnlist, cObsermaticConfPar& confpar, bool demo);
    virtual ~cRangeObsermatic();
    virtual void generateInterface(); // here we export our interface (entities)
    cVeinModuleComponent *m_pRangingSignal;
public slots:
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);
    void catchChannelNewRangeList();
    void demoActValues(QVector<float> *actualValues);
private:
    bool m_bDemo;
    cRangeModule *m_pModule;
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    cSocket *m_pDSPSocket;
    QList<QStringList> m_GroupList;
    QStringList m_ChannelNameList; // the system names of our channels
    cObsermaticConfPar& m_ConfPar;
    Zera::Server::cDSPInterface* m_pDSPInterFace; // our interface to dsp
    Zera::Proxy::cProxyClient *m_pDspClient;
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
    QList<cVeinModuleParameter*> m_RangeParameterList;
    QList<cVeinModuleComponent*> m_RangeOVLComponentList;
    QList<cVeinModuleComponent*> m_RangeOVLRejectionComponentList;
    QList<cVeinModuleComponent*> m_RangeActRejectionComponentList;
    QList<cVeinModuleComponent*> m_RangeActOVLRejectionComponentList;
    QList<cVeinModuleParameter*> m_RangeGroupPreScalingList;
    QList<cVeinModuleParameter*> m_RangeGroupPreScalingEnabledList;
    QList<cVeinModuleComponent*> m_RangeGroupPreScalingInfo;

    cVeinModuleParameter* m_pParRangeAutomaticOnOff;
    cVeinModuleParameter* m_pParGroupingOnOff;
    cVeinModuleParameter* m_pParOverloadOnOff;
    cVeinModuleComponent* m_pComponentOverloadMax;

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

    QTimer m_DemoTimer;

    bool m_brangeSet; // we set this here after we selected a new range and enable resetting stored overloadcondition

    void rangeObservation();
    void rangeAutomatic();
    void groupHandling();
    void setRanges(bool force = false); // here we really set ranges
    QList<int> getGroupIndexList(int index);
    bool requiresOverloadReset(int channel);
    void startOverloadReset(int channel);
    void setupDemoOperation();
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
    void demoTimerTimeout();
};

}

#endif // RANGEOBSERMATIC_H
