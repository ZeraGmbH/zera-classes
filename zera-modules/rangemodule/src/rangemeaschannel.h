#ifndef RANGEMEASCHANNEL_H
#define RANGEMEASCHANNEL_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QVariant>
#include <pcbinterface.h>

#include "reply.h"
#include "basemeaschannel.h"
#include "rangeinfo.h"

namespace RANGEMODULE
{

enum rangemeaschannelCmds
{
    sendmeaschannelrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    claimresource,
    freeresource,
    readdspchannel,
    readchnalias,
    readsamplerate,
    readunit,
    readrangelist,

    readrngalias,
    readtype,
    readurvalue,
    readrejection,
    readovrejection,
    readisavail,

    setmeaschannelrange,
    readgaincorrection,
    readoffsetcorrection,
    readphasecorrection,
    readmeaschannelstatus,
    resetmeaschannelstatus,
    resetmeaschannelstatus2, // we use this her in state machine while activating
};

const double sqrt2 = 1.41421356;

class cRangeMeasChannel:public cBaseMeasChannel
{
    Q_OBJECT

public:
    cRangeMeasChannel(Zera::Proxy::cProxy* proxy, VeinPeer *peer, cSocket* rmsocket, cSocket* pcbsocket, QString name, quint8 chnnr);
    ~cRangeMeasChannel();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration
    quint32 setRange(QString range); // a statemachine gets started that returns cmdDone(quint32 cmdnr)
    quint32 readGainCorrection(double amplitude); // dito
    quint32 readPhaseCorrection(double frequency); // dito
    quint32 readOffsetCorrection(double amplitude); // dito
    quint32 readStatus(); // read the channels status
    quint32 resetStatus(); // reset the channels status
    double getGainCorrection(); // returns last read gain correction
    double getPhaseCorrection(); // returns last read phase correction
    double getOffsetCorrection(); // returns last read offset correction
    bool isHWOverload(); // test if we have some hardware overload condition
    double getUrValue(QString range); // returns upper range value of range
    double getUrValue(); // returns upper range of actual range
    double getRejection(QString range);
    double getRejection(); // return nominal (100%) rejection of actual range
    double getOVRRejection(QString range);
    double getOVRRejection();
    double getMaxRangeUrvalueMax(); // returns the max. upper range value including reserve of channel including overload reserve
    double getRangeUrvalueMax(); // returns the max. upper range value including reserve of actual range
    bool isPossibleRange(QString range, double ampl); // test if range is possible with ampl
    bool isPossibleRange(QString range); // returns true if range is available
    bool isOverload(double ampl); // test if ampl is overload condition

    QString getOptRange(double ampl); // returns opt. range alias
    QString getMaxRange(); // returns alias of the range with max ur value

signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready


protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    VeinEntity *m_pChannelEntity;
    VeinEntity *m_pChannelRangeListEntity;
    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfo> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QString m_sNewRange;
    QString m_sActRange; // the actual range set (alias)
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;
    quint32 m_nStatus;

    // statemachine for activating a rangemeaschannel
    QState m_rmConnectState; // we must connect first to resource manager
    QState m_IdentifyState; // we must identify ourself at resource manager
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for our resource needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_claimResourceState; // we claim our resource
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_readDspChannelState; // we query our dsp channel
    QState m_readChnAliasState; // we query our alias
    QState m_readSampleRateState; // we read the sample nr
    QState m_readUnitState; // we read the meas channel unit volt ampere ...
    QState m_readRangelistState; // we query our range list
    QState m_readRangeProperties1State; // we build up a loop for querying all the ranges properties
    QState m_readRangeProperties2State; //
    QState m_readRangeProperties3State;
    QState m_resetStatusState; // we reset our measuring channel when activated
    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;


    // statemachine for querying a measchannels range properties
    QStateMachine m_rangeQueryMachine;
    QState m_readRngAliasState;
    QState m_readTypeState;
    QState m_readUrvalueState;
    QState m_readRejectionState;
    QState m_readOVRejectionState;
    QState m_readisAvailState;
    QFinalState m_rangeQueryDoneState;

    // statemachine for querying a measchannels range properties
    QStateMachine m_freeResourceStatemachine;
    QState m_freeResourceStartState;
    QFinalState m_freeResourceDoneState;

    void setRangeListEntity();
    void setChannelNameEntity();

    qint32 m_RangeQueryIt;
    cRangeInfo ri;

    Zera::Proxy::cProxyClient* m_pRMClient;
    Zera::Proxy::cProxyClient* m_pPCBClient;

private slots:
    void rmConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void claimResource();
    void pcbConnection();
    void readDspChannel();
    void readChnAlias();
    void readSampleRate();
    void readUnit();
    void readRangelist();
    void readRangeProperties1();
    void readRangeProperties3();
    void resetStatusSlot();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    // the slots for querying a measchannels range properties
    void readRngAlias();
    void readType();
    void readUrvalue();
    void readRejection();
    void readOVRejection();
    void readisAvail();
    void rangeQueryDone();
};

}

#endif // RANGEMEASCHANNEL_H
