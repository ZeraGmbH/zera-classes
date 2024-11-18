#ifndef PCBCHANNELMANAGER_H
#define PCBCHANNELMANAGER_H

#include "pcbchanneldata.h"
#include <pcbinterface.h>
#include <proxyclient.h>
#include <taskcontainerinterface.h>
#include <QMap>

class PcbChannelManager : public QObject
{
    Q_OBJECT
public:
    void startScan(Zera::ProxyClientPtr pcbClient);
    const QStringList getChannelMNames() const;
    const QStringList getChannelRanges(const QString &channelMName) const;
    const PcbChannelDataPtr getChannelData(QString channelMName);
signals:
    void sigScanFinished(bool ok);
    void sigChannelRangesChanged(QString channelMName);

protected:
    QMap<QString, PcbChannelDataPtr> m_channelNamesToData;
    QHash<int, QString> m_notifyIdToChannelMName;

private slots:
    void onTasksFinish(bool ok);
    void onInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    void prepareScan(Zera::ProxyClientPtr pcbClient);
    void startRangesChangeTasks(QString channelMName);
    static void notifyError(QString errMsg);
    TaskTemplatePtr getChannelsReadInfoAndRangeNamesTasks(const QStringList &channelMNames);
    TaskTemplatePtr getChannelReadRangeNamesTask(const QString &channelMName);
    static TaskTemplatePtr getReadRangeDetailsTasks(const QStringList &channelMNames);
    TaskTemplatePtr getReadRangeFinalTasks(const QStringList &channelMNames);
    TaskContainerInterfacePtr m_currentTasks;
    Zera::PcbInterfacePtr m_currentPcbInterface;

    QStringList m_tempChannelMNames;
};

#endif // PCBCHANNELMANAGER_H
