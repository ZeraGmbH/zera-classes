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
    const PcbChannelDataPtr getChannelData(QString channelName);
signals:
    void sigScanFinished(bool ok);
    void sigChannelChanged(QString channelMName);

protected:
    QMap<QString, PcbChannelDataPtr> m_channels;

private slots:
    void onTasksFinish(bool ok);
private:
    void createTasks(Zera::ProxyClientPtr pcbClient);
    void notifyError(QString errMsg);
    TaskTemplatePtr getChannelsReadTasks(Zera::PcbInterfacePtr pcbInterface);
    TaskContainerInterfacePtr m_currentTasks;

    QStringList m_tempChannelMNames;
    QMap<QString, QStringList> m_tempChannelRanges;
};

#endif // PCBCHANNELMANAGER_H
