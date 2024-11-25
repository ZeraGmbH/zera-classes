#ifndef CHANNEL_RANGE_OBSERVER_RANGE_H
#define CHANNEL_RANGE_OBSERVER_RANGE_H

#include <abstracttcpnetworkfactory.h>
#include <networkconnectioninfo.h>
#include <pcbinterface.h>
#include <tasktemplate.h>

namespace ChannelRangeObserver {

class Range : public QObject
{
    Q_OBJECT
public:
    bool m_available = false;
    double m_urValue = 0.0;

    Range(const QString &channelMName,
          const NetworkConnectionInfo &netInfo,
          VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFetch();
private:
    TaskTemplatePtr getPcbConnectionTask();
    const QString m_channelMName;

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    const Zera::PcbInterfacePtr m_pcbInterface;
};

}

#endif // CHANNEL_RANGE_OBSERVER_RANGE_H
