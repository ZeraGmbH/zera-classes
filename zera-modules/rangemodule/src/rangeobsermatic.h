#ifndef RANGEOBSERMATIC_H
#define RANGEOBSERMATIC_H


// cRangeObsermatic is responsible for setting, observing, automatic setting
// and grouping measurement channel ranges. it works on a flexible list of
// system measurement channels.
// it provides its interface :
// ChannelXRange Input/Output for range setting
// ChannelXOVR   Output information of overload conditions
// RangeAutomatic ON/OFF Input
// GroupHandling ON/OFF Input
// !!! important range obsermatic must be activated after !!! activating rangemeaschannels

#include <QObject>
#include <QList>
#include <QVector>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QVariant>
#include <QStringList>

#include "moduleactivist.h"
#include "rangemoduleconfigdata.h"


namespace Zera {
namespace Proxy {
    class cProxy;
    class cProxyClient;
}
namespace  Server {
    class cDSPInterface;
}
}


class VeinPeer;
class VeinEntity;
class cDspMeasData;

class cModuleParameter;
class cModuleSignal;
class cModuleInfo;

namespace RANGEMODULE
{

enum rangeObsermaticCmds
{
    readgain2corr,
    writegain2corr,
    setrange,
    resetstatus,
    readstatus
};


class cRangeModule;
class cRangeMeasChannel;

class cRangeObsermatic: public cModuleActivist
{
    Q_OBJECT

public:
    cRangeObsermatic(cRangeModule* module, Zera::Proxy::cProxy* proxy, VeinPeer* peer, cSocket* dspsocket, QList<QStringList> groupList, QStringList chnlist, cObsermaticConfPar& confpar);
    virtual ~cRangeObsermatic();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

    cModuleSignal *m_pRangingSignal; // we make the signal public to easy connection within module

public slots:
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);

signals:
    void readStatusContinue();

private:
    cRangeModule *m_pModule;
    Zera::Proxy::cProxy* m_pProxy; // the proxy where we can get our connections
    VeinPeer* m_pPeer;
    cSocket *m_pDSPSocket;
    QList<QStringList> m_GroupList;
    QStringList m_ChannelNameList; // the system names of our channels
    cObsermaticConfPar& m_ConfPar;
    Zera::Server::cDSPInterface* m_pDSPInterFace; // our interface to dsp
    Zera::Proxy::cProxyClient *m_pDspClient;
    QStringList m_ChannelAliasList; // the alias of our channels
    QList<cRangeMeasChannel*> m_RangeMeasChannelList;
    QList<bool> m_softOvlList; // here we enter software detected overloads
    QList<bool> m_hardOvlList; // what do you think ?
    QList<bool> m_maxOvlList; // here we enter overloads that occured in maximum range
    quint32 m_nRangeSetPending;
    quint32 m_nReadStatusPending;
    QStringList m_actChannelRangeList; // a list of the actual ranges set
    QVector<float> m_ActualValues; // here we find the actual values

    // our interface entities
    QList<VeinEntity*> m_RangeEntityList;
    QList<VeinEntity*> m_RangeOVLEntityList;
    QList<VeinEntity*> m_RangeRejectionEntityList;
    QList<VeinEntity*> m_RangeActRejectionEntityList;
    QList<VeinEntity*> m_RangeActOvrRejectionEntityList;
    QList<cModuleInfo*> m_GroupInfoList;

    cModuleParameter* m_pParRangeAutomaticOnOff;
    cModuleParameter* m_pParGroupingOnOff;
    cModuleParameter* m_pParOverloadOnOff;

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
    QFinalState m_writeGainCorrDoneState;

    // statemachine for reading status of all measurement channels
    QStateMachine m_readStatusMachine;
    QState m_readStatusState;
    QFinalState m_analyzeStatusState;

    bool m_bRangeAutomatic;
    bool m_bGrouping;
    bool m_brangeSet; // we set this here after we selected a new range and enable resetting stored overloadcondition

    void rangeObservation();
    void rangeAutomatic();
    void groupHandling();
    void setRanges(bool force = false); // here we really set ranges
    QList<int> getGroupIndexList(int index);

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

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant);
};

}

#endif // RANGEOBSERMATIC_H
