#ifndef CRO_CHANNEL_OBSERVER_H
#define CRO_CHANNEL_OBSERVER_H

#include "cro_channel.h"
#include <pcbinterface.h>
#include <proxyclient.h>
#include <taskcontainerinterface.h>
#include <QMap>

namespace ChannelRangeObserver {

class SystemObserver : public QObject
{
    Q_OBJECT
public:
    SystemObserver(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFullScan();
    const QStringList getChannelMNames() const;
    const QStringList getChannelAliases() const;
    const ChannelPtr getChannel(QString channelMName) const;
    const int getSamplesPerPeriod() const;
signals:
    void sigFullScanFinished(bool ok);
    void sigFetchDone(QString channelMName, bool ok);
    friend class SystemObserverResetter;

protected:
    QMap<QString, ChannelPtr> m_channelMNameToChannel;

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

    std::shared_ptr<int> m_samplesPerPeriod = std::make_shared<int>(0);
    TaskContainerInterfacePtr m_currentTasks;
    std::shared_ptr<QStringList> m_tempChannelMNames;
};

typedef std::shared_ptr<SystemObserver> SystemObserverPtr;

}
#endif // CRO_CHANNEL_OBSERVER_H
