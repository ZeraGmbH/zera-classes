#ifndef AllCHANNELSOBSERVER_H
#define AllCHANNELSOBSERVER_H

#include "channelbserver.h"
#include <pcbinterface.h>
#include <proxyclient.h>
#include <taskcontainerinterface.h>
#include <QMap>

class AllChannelsObserver : public QObject
{
    Q_OBJECT
public:
    AllChannelsObserver(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFullScan();
    const QStringList getChannelMNames() const;
    const ChannelObserverPtr getChannelObserver(QString channelMName);
signals:
    void sigFullScanFinished(bool ok);
    void sigFetchComplete(QString channelMName);
    friend class AllChannelsObserverResetter;

protected:
    QMap<QString, ChannelObserverPtr> m_channelNamesToObserver;

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

#endif // AllCHANNELSOBSERVER_H
