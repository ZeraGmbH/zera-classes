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

#include "moduleacitvist.h"

namespace RANGEOBSERMATIC
{

enum rangeObsermaticCmds
{
    readgain2corr,
    writegain2corr,
    setrange
};
}

class VeinPeer;
class VeinEntity;
class cRangeModule;
class cModuleParameter;
class cModuleSignal;
class cDspMeasData;
class cModuleInfo;
class cRangeMeasChannel;

namespace Zera {
namespace Server {
    class cDSPInterface;
}
}

class cRangeObsermatic: public cModuleActivist
{
    Q_OBJECT

public:
    cRangeObsermatic(cRangeModule* module, VeinPeer* peer, Zera::Server::cDSPInterface* iface, QList<QStringList> groupList, QStringList chnlist, QStringList rnglist, quint8 rangeauto, quint8 grouping);
    virtual ~cRangeObsermatic();

    cModuleSignal *m_pRangingSignal; // we make the signal public to easy connection within module

public slots:
    virtual void activate(); // here we query our properties and activate ourself
    virtual void deactivate(); // what do you think ? yes you're right
    virtual void ActionHandler(QVector<float>* actualValues); // entry after received actual values
    void catchChannelReply(quint32 msgnr);

signals:
    void activationContinue();

protected:
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

private:
    cRangeModule *m_pModule;
    VeinPeer* m_pPeer;
    Zera::Server::cDSPInterface* m_pDSPIFace; // our interface to dsp
    QList<QStringList> m_GroupList;
    QStringList m_ChannelNameList; // the system names of our channels
    QStringList m_ChannelAliasList; // the alias of our channels
    QList<cRangeMeasChannel*> m_RangeMeasChannelList;
    QList<bool> m_OvlList;
    quint32 m_nRangeSetPending;
    QStringList m_actChannelRangeList; // a list of the actual ranges set
    QStringList m_newChannelRangeList; // a list of new ranges we want to get set
    QVector<float> m_ActualValues; // here we find the actual values

    // our interface entities
    QList<VeinEntity*> m_RangeEntityList;
    QList<VeinEntity*> m_RangeOVLEntityList;
    QList<cModuleInfo*> m_GroupInfoList;
    cModuleParameter* m_pParRangeAutomaticOnOff;
    cModuleParameter* m_pParGroupingOnOff;

    cDspMeasData* m_pGainCorrection2DSP; // copy of dsp internal correction data
    float* m_pfScale;
    QHash<quint32, int> m_MsgNrCmdList;

    // statemachine for reading dsp correction data used for scaling
    QStateMachine m_readCorrectionDSPMachine;
    QState m_readGainCorrState;
    QFinalState m_readGainCorrDoneState;

    // statemachine for writing dsp correction data used for scaling
    QStateMachine m_writeCorrectionDSPMachine;
    QState m_writeGainCorrState;
    QFinalState m_writeGainCorrDoneState;

    quint8 m_nDefaultRangeAuto;
    quint8 m_nDefaultGrouping;
    bool m_bRangeAutomatic;
    bool m_bGrouping;
    void rangeObservation();
    void rangeAutomatic();
    void groupHandling();
    void setRanges(); // here we really set ranges

private slots:
    void readGainCorr();
    void readGainCorrDone();
    void writeGainCorr();
    void writeGainCorrDone();

    void newRange(QVariant range);
    void newRangeAuto(QVariant rauto);
    void newGrouping(QVariant rgrouping);

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant);
};

#endif // RANGEOBSERMATIC_H
