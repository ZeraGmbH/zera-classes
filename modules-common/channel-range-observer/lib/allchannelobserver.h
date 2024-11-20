#ifndef AllCHANNELOBSERVER_H
#define AllCHANNELOBSERVER_H

#include "rangeobserver.h"
#include <pcbinterface.h>
#include <proxyclient.h>
#include <taskcontainerinterface.h>
#include <QMap>

class AllChannelObserver : public QObject
{
    Q_OBJECT
public:
    AllChannelObserver(const NetworkConnectionInfo &netInfo, VeinTcp::AbstractTcpNetworkFactoryPtr tcpFactory);
    void startFullScan();
    const QStringList getChannelMNames() const;
    const QStringList getChannelRanges(const QString &channelMName) const;
    const RangeObserverPtr getChannelData(QString channelMName);
signals:
    void sigFullScanFinished(bool ok);
    void sigRangeListChanged(QString channelMName);
    friend class AllChannelObserverResetter;

protected:
    QMap<QString, RangeObserverPtr> m_channelNamesToObserver;
    QHash<int, QString> m_notifyIdToChannelMName;

private:
    void clear();
    void doStartFullScan();
    TaskTemplatePtr perparePcbConnectionTasks();
    TaskTemplatePtr getChannelReadDetailsTasks(const QString &channelMName, int notificationId);
    void startRangesChangeTasks(QString channelMName);
    static void notifyError(QString errMsg);

    NetworkConnectionInfo m_netInfo;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;

    TaskContainerInterfacePtr m_currentTasks;
    QStringList m_tempChannelMNames;
};

#endif // AllCHANNELOBSERVER_H
