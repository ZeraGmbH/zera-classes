#ifndef RANGEMEASCHANNEL_H
#define RANGEMEASCHANNEL_H

#include "rangechanneldata.h"
#include <pcbinterface.h>
#include <basemeaschannel.h>
#include <rangeinfo.h>
#include <QFinalState>

namespace RANGEMODULE
{

class cRangeMeasChannel: public cBaseMeasChannel
{
    Q_OBJECT
public:

    const double keepRangeLimit = 0.99;
    const double enterRangeLimit = 0.95;

    cRangeMeasChannel(const ChannelRangeObserver::ChannelPtr &channelObserver,
                      const NetworkConnectionInfo &pcbsocket,
                      const VeinTcp::AbstractTcpNetworkFactoryPtr &tcpNetworkFactory,
                      const QString &moduleName);
    void generateVeinInterface() override;

    RangeChannelData* getSharedChannelData();

    QString getCurrRangeName() const;
    quint32 setRange(const QString &rangeName);
    quint32 readGainCorrection(double amplitude);
    quint32 readPhaseCorrection(double frequency);
    quint32 readOffsetCorrection(double amplitude);
    quint32 readStatus(); // read the channels status
    quint32 resetStatus(); // reset the channels status

    double getGainCorrection(); // returns last read gain correction
    double getPhaseCorrection(); // returns last read phase correction
    double getOffsetCorrection(); // returns last read offset correction

    bool isHWOverload(); // test if we have some hardware overload condition

    double getUrValue(const QString &rangeName) const;      // upper range value
    double getRejection(const QString &rangeName) const;    // nominal (100%) rejection
    double getOVRRejection(const QString &rangeName) const; // over (usually 125%) rejection
    bool isPossibleRange(const QString &rangeName); // returns true if range is available
    bool isRMSOverload(double ampl); // test if ampl is overload condition
    bool isPeakOverload(double ampl); // test if ampl is adc overload condition

    QString getOptimalRange(double rms, double peak, const QString &rangeName); // returns opt. range name
    QString getMaxRangeName() const;
    QString getMaxRangeName(const QString &rangeName); // returns name of the range with max ur value
    QString getAvailAliasList(); // public (Vein / SCPI)

signals:
    void cmdDone(quint32 cmdnr); // to signal we are ready
    void newRangeList(); // if the channel has read new range list after async. notification

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void pcbConnection();
    void readRangeAndProperties();
    void resetStatusSlot();
    void setNotifierRangeCat();
    void activationDone();

    void deactivationResetNotifiers();
    void deactivationDone();

    // the slots for querying a measchannels range properties
    void readRangelist();
    void readType();
    void readUrvalue();
    void readRejection();
    void readOVRejection();
    void readADCRejection();
    void readisAvail();
    void rangeQueryLoop();
private:
    void setSortedRangeNameList();
    QString findOptimalRange(double actualValue, const QString &actRangeName, qint32 actRngType, double extraRejectionFactor);

    RangeChannelData m_sharedChannelData;
    QStringList m_allRangeNamesOrderedByServer; // a list of all ranges

    cRangeInfoWithConstantValues m_oneWorkRangeInfo;
    QHash<QString, cRangeInfoWithConstantValues> m_growingWorkRangeInfo;
    QHash<QString, cRangeInfoWithConstantValues> m_rangeNameToChannelData;

    QString m_nextRangeName;
    QString m_actRangeName;
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;
    quint32 m_statusBitMask; // currently just COM5003 HW overload bit
    QString m_sortedRangeNameList;

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
    QState m_readTypeState;
    QState m_readUrvalueState;
    QState m_readRejectionState;
    QState m_readOVRejectionState;
    QState m_readADCRejectionState;
    QState m_readisAvailState;
    QState m_rangeQueryLoopState;
    QFinalState m_rangeQueryDoneState;

    qint32 m_RangeQueryIt;
};


}

#endif // RANGEMEASCHANNEL_H
