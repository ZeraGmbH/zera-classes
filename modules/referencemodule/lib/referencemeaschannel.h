#ifndef REFERENCEMEASCHANNEL_H
#define REFERENCEMEASCHANNEL_H

#include <rangeinfo.h>
#include <QHash>
#include <QStringList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QVariant>
#include "basemeaschannel.h"

namespace REFERENCEMODULE
{

enum rangemeaschannelCmds
{
    readchnalias,
    readunit,
    readrangelist,

    readrngalias,
    readtype,
    readisavail,

    setmeaschannelrange,
    readgaincorrection,
    readoffsetcorrection,
    readphasecorrection,
    readmeaschannelstatus,
    resetmeaschannelstatus
};

class cReferenceMeasChannel: public cBaseMeasChannel
{
    Q_OBJECT

public:
    cReferenceMeasChannel(ChannelRangeObserver::ChannelPtr channelObserver,
                          NetworkConnectionInfo pcbsocket, VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                          quint8 chnnr, QString moduleName);
    void generateVeinInterface() override;

    quint32 setRange(QString range); // a statemachine gets started that returns cmdDone(quint32 cmdnr)

signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready

protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) override;

private:
    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfoBase> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QString m_sNewRange;
    QString m_sActRange; // the actual range set (alias)
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;
    quint32 m_nStatus;

    // statemachine for activating a referencemeaschannel
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_readChnAliasState; // we query our alias
    QState m_readUnitState; // we read the meas channel unit volt ampere ...
    QState m_readRangelistState; // we query our range list
    QState m_readRangeProperties1State; // we build up a loop for querying all the ranges properties
    QState m_readRangeProperties2State; //
    QState m_readRangeProperties3State;
    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;

    // statemachine for querying a measchannels range properties
    QStateMachine m_rangeQueryMachine;
    QState m_readRngAliasState;
    QState m_readTypeState;
    QState m_readisAvailState;
    QFinalState m_rangeQueryDoneState;

    qint32 m_RangeQueryIt;
    cRangeInfoBase m_rangeInfo;

    Zera::ProxyClientPtr m_rmClient;
    Zera::ProxyClientPtr m_pcbClient;

private slots:
    void pcbConnection();
    void readChnAlias();
    void readUnit();
    void readRangelist();
    void readRangeProperties1();
    void readRangeProperties3();
    void activationDone();

    void deactivationInit();
    void deactivationDone();

    // the slots for querying a measchannels range properties
    void readRngAlias();
    void readType();
    void readisAvail();
    void rangeQueryDone();
};

}

#endif // REFERENCEMEASCHANNEL_H
