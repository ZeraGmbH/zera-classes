#ifndef CHANNELRANGEOBSERVER_H
#define CHANNELRANGEOBSERVER_H

#include "channelobserver.h"
#include <pcbinterface.h>
#include <proxyclient.h>
#include <taskcontainerinterface.h>
#include <QMap>

class ChannelRangeObserver : public QObject
{
    Q_OBJECT
public:
    void startFullScan(Zera::ProxyClientPtr pcbClient);
    const QStringList getChannelMNames() const;
    const QStringList getChannelRanges(const QString &channelMName) const;
    const ChannelObserverPtr getChannelData(QString channelMName);
signals:
    void sigScanFinished(bool ok);
    void sigRangeListChanged(QString channelMName);

protected:
    QMap<QString, ChannelObserverPtr> m_channelNamesToObserver;
    QHash<int, QString> m_notifyIdToChannelMName;

private slots:
    void onTasksFinish(bool ok);
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void doStartFullScan(Zera::ProxyClientPtr pcbClient);
    TaskTemplatePtr perparePcbConnectionTasks(Zera::ProxyClientPtr pcbClient);
    TaskTemplatePtr getChannelReadDetailsTasks(const QString &channelMName, int notificationId);
    void startRangesChangeTasks(QString channelMName);
    static void notifyError(QString errMsg);
    TaskContainerInterfacePtr m_currentTasks;
    Zera::PcbInterfacePtr m_currentPcbInterface;
};

#endif // CHANNELRANGEOBSERVER_H
