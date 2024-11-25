#ifndef CHANNEL_RANGE_OBSERVER_All_CHANNELS_H
#define CHANNEL_RANGE_OBSERVER_All_CHANNELS_H

#include "channel.h"
#include <pcbinterface.h>
#include <proxyclient.h>
#include <taskcontainerinterface.h>
#include <QMap>

namespace ChannelRangeObserver {

class AllChannels : public QObject
{
    Q_OBJECT
public:
    AllChannels(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFullScan();
    const QStringList getChannelMNames() const;
    const ChannelPtr getChannel(QString channelMName);
signals:
    void sigFullScanFinished(bool ok);
    void sigFetchComplete(QString channelMName);
    friend class AllChannelsResetter;

protected:
    QMap<QString, ChannelPtr> m_channelNameToChannel;

private:
    void clear();
    void doStartFullScan();
    TaskTemplatePtr getPcbConnectionTask();
    static void notifyError(QString errMsg);

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    const Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
    QStringList m_tempChannelMNames;
};

}
#endif // CHANNEL_RANGE_OBSERVER_All_CHANNELS_H
