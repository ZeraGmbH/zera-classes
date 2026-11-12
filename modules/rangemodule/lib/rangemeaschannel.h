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
    void generateVeinInterface() override {};

    // setter
    quint32 setRange(const QString &rangeName);

    // getter
    RangeChannelData* getSharedChannelData();
    QString getCurrRangeName() const;

    // for channel adjustment
    quint32 readGainCorrection(double amplitude);
    quint32 readPhaseCorrection(double frequency);
    quint32 readOffsetCorrection(double amplitude);
    double getGainCorrection(); // returns last read gain correction
    double getPhaseCorrection(); // returns last read phase correction
    double getOffsetCorrection(); // returns last read offset correction

    // for range obsermatic
    quint32 readStatus(); // read the channels status
    quint32 resetStatus(); // reset the channels status
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
    void resetStatusSlot();
    void activationDone();

    void deactivationDone();

private:
    QString findOptimalRange(double actualValue, const QString &actRangeName, qint32 suggestedRangeType, double extraRejectionFactor);
    qint32 suggestTypeForNextRange(const QString &rangeName);

    RangeChannelData m_sharedChannelData;

    QString m_nextRangeName;
    QString m_actRangeName;
    double m_fGainCorrection;
    double m_fPhaseCorrection;
    double m_fOffsetCorrection;
    quint32 m_statusBitMask; // currently just COM5003 HW overload bit

    // statemachine for activating a rangemeaschannel
    QState m_pcbConnectionState; // we try to get a connection to our pcb server
    QState m_resetStatusState; // we reset our measuring channel when activated
    QFinalState m_activationDoneState;

    // statemachine for deactivating a rangemeaschannel
    QFinalState m_deactivationDoneState;
};

}

#endif // RANGEMEASCHANNEL_H
