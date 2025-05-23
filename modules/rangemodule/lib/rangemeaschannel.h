#ifndef RANGEMEASCHANNEL_H
#define RANGEMEASCHANNEL_H

#include "rangechanneldata.h"
#include <pcbinterface.h>
#include <basemeaschannel.h>
#include <rangeinfo.h>
#include <QFinalState>

namespace RANGEMODULE
{

enum rangemeaschannelCmds
{
    readrangelist,

    readrngalias,
    readtype,
    readurvalue,
    readrejection,
    readovrejection,
    readadcrejection,
    readisavail,

    setmeaschannelrange,
    readgaincorrection,
    readoffsetcorrection,
    readphasecorrection,
    readmeaschannelstatus,
    resetmeaschannelstatus,
    resetmeaschannelstatus2, // we use this her in state machine while activating
    registerNotifier,
    unregisterNotifiers
};

class cRangeMeasChannel: public cBaseMeasChannel
{
    Q_OBJECT
public:

    const double keepRangeLimit = 0.99;
    const double enterRangeLimit = 0.95;

    cRangeMeasChannel(ChannelRangeObserver::ChannelPtr channelObserver,
                      NetworkConnectionInfo pcbsocket,
                      VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                      QString moduleName);
    void generateVeinInterface() override;

    RangeChannelData* getChannelData();

    QString getRange() const;
    quint32 setRange(const QString &range); // a statemachine gets started that returns cmdDone(quint32 cmdnr)
    quint32 readGainCorrection(double amplitude); // dito
    quint32 readPhaseCorrection(double frequency); // dito
    quint32 readOffsetCorrection(double amplitude); // dito
    quint32 readStatus(); // read the channels status
    quint32 resetStatus(); // reset the channels status

    double getGainCorrection(); // returns last read gain correction
    double getPhaseCorrection(); // returns last read phase correction
    double getOffsetCorrection(); // returns last read offset correction

    bool isHWOverload(); // test if we have some hardware overload condition

    double getUrValue(const QString &range) const;      // upper range value
    double getRejection(const QString &range) const;    // nominal (100%) rejection
    double getOVRRejection(const QString &range) const; // over (usually 125%) rejection
    bool isPossibleRange(const QString &range); // returns true if range is available
    bool isRMSOverload(double ampl); // test if ampl is overload condition
    bool isPeakOverload(double ampl); // test if ampl is adc overload condition

    QString getOptRange(double rms, double peak, const QString &rngAlias); // returns opt. range alias
    QString getMaxRange() const; // returns alias of the range with max ur value
    QString getMaxRange(const QString &rngAlias); // returns alias of the range with max ur value
    QString getRangeListAlias();

signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready
    void newRangeList(); // if the channel has read new range list after async. notification

protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) override;

private:
    RangeChannelData m_channelData;
    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfoWithConstantValues> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QHash<QString, cRangeInfoWithConstantValues> m_RangeInfoHashWorking;
    QString m_sNewRange;
    QString m_sActRange; // the actual range set (alias)
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;
    quint32 m_nStatus;
    QString m_sRangeListAlias; // all range alias: alias1;alias2 ....

    // statemachine for activating a rangemeaschannel
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_readRangeAndProperties; // we query our range list and all their properties
    QState m_resetStatusState; // we reset our measuring channel when activated
    QState m_setNotifierRangeCat; // we request notification for range cat query
    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QState m_deactivationInitState;
    QFinalState m_deactivationDoneState;


    // statemachine for querying a measchannels range properties
    QStateMachine m_rangeQueryMachine;
    QState m_readRangelistState; // we query our range list
    QState m_readRngAliasState;
    QState m_readTypeState;
    QState m_readUrvalueState;
    QState m_readRejectionState;
    QState m_readOVRejectionState;
    QState m_readADCRejectionState;
    QState m_readisAvailState;
    QState m_rangeQueryLoopState;
    QFinalState m_rangeQueryDoneState;

    qint32 m_RangeQueryIt;
    cRangeInfoWithConstantValues m_CurrRangeInfo;

    Zera::ProxyClientPtr m_pcbClient;

    void setRangeListAlias();
    QString findOptimalRange(double actualValue, const QString &actRngAlias, qint32 actRngType, double extraRejectionFactor);

private slots:
    void pcbConnection();
    void readRangeAndProperties();
    void resetStatusSlot();
    void setNotifierRangeCat();
    void activationDone();

    void deactivationResetNotifiers();
    void deactivationDone();

    // the slots for querying a measchannels range properties
    void readRangelist();
    void readRngAlias();
    void readType();
    void readUrvalue();
    void readRejection();
    void readOVRejection();
    void readADCRejection();
    void readisAvail();
    void rangeQueryLoop();
};

}

#endif // RANGEMEASCHANNEL_H
