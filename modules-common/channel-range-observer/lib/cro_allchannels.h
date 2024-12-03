#ifndef CRO_All_CHANNELS_H
#define CRO_All_CHANNELS_H

#include "cro_channel.h"
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
    const ChannelPtr getChannel(QString channelMName) const;
signals:
    void sigFullScanFinished(bool ok);
    void sigFetchComplete(QString channelMName, bool ok);
    friend class AllChannelsResetter;

protected:
    QMap<QString, ChannelPtr> m_channelNameToChannel;

private:
    void clear();
    void preparePcbInterface();
    void doStartFullScan();
    TaskTemplatePtr getPcbConnectionTask();
    static void notifyError(QString errMsg);

    const NetworkConnectionInfo m_netInfo;
    const VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    const Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
    QStringList m_tempChannelMNames;
};

}
#endif // CRO_All_CHANNELS_H
