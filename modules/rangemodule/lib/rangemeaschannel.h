#ifndef RANGEMEASCHANNEL_H
#define RANGEMEASCHANNEL_H

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
    cRangeMeasChannel(ChannelRangeObserver::ChannelPtr channelObserver,
                      NetworkConnectionInfo pcbsocket,
                      VeinTcp::AbstractTcpNetworkFactoryPtr tcpNetworkFactory,
                      QString moduleName);
    void generateVeinInterface() override;

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
    double getUrValue(const QString &range); // returns upper range value of range
    double getUrValue(); // returns upper range of actual range
    double getRejection(const QString &range);
    double getRejection(); // return nominal (100%) rejection of actual range
    double getOVRRejection(const QString &range);
    double getOVRRejection();
    double getMaxRangeUrvalueMax(); // returns the max. upper range value including reserve of channel including overload reserve
    double getRangeUrvalueMax(); // returns the max. upper range value including reserve of actual range
    bool isPossibleRange(const QString &range); // returns true if range is available
    bool isRMSOverload(double ampl); // test if ampl is overload condition
    bool isADCOverload(double ampl); // test if ampl is adc overload condition

    QString getOptRange(double rms, const QString &rngAlias); // returns opt. range alias
    QString getMaxRange(); // returns alias of the range with max ur value
    QString getMaxRange(const QString &rngAlias); // returns alias of the range with max ur value
    QString getRangeListAlias();

    double getPreScaling() const;
    void setPreScaling(double preScaling);

    double getRmsValue() const;
    void setRmsValue(double actualValue);

    double getSignalFrequency() const;
    void setSignalFrequency(double signalFrequency);

    double getPeakValue() const;
    void setPeakValue(double peakValue);

    double getPeakValueWithDc() const;
    void setPeakValueWithDc(double peakValueWithDc);

    void setInvertedPhaseState(bool inverted);
    bool getInvertedPhaseState();

signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready
    void newRangeList(); // if the channel has read new range list after async. notification

protected slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer) override;

private:
    QStringList m_RangeNameList; // a list of all ranges
    QHash<QString, cRangeInfoWithConstantValues> m_RangeInfoHash; // a list of available and selectable ranges, alias will be the key
    QHash<QString, cRangeInfoWithConstantValues> m_RangeInfoHashWorking;
    QHash<int, int> m_ActionErrorcountHash;
    QString m_sNewRange;
    QString m_sActRange; // the actual range set (alias)
    double m_peakValue;
    double m_peakValueWithDc;
    double m_rmsValue;
    double m_signalFrequency;
    double m_preScaling;
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;
    quint32 m_nStatus;
    QString m_sRangeListAlias; // all range alias: alias1;alias2 ....
    bool m_invertedPhase = false;

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
    void setActionErrorcount(int Count);

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
