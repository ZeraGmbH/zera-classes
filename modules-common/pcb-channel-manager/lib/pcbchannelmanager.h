#ifndef PCBCHANNELMANAGER_H
#define PCBCHANNELMANAGER_H

#include <pcbinterface.h>
#include <proxyclient.h>
#include <taskcontainerinterface.h>
#include <QMap>

class PcbChannelManager : public QObject
{
    Q_OBJECT
public:
    void startScan(Zera::ProxyClientPtr pcbClient);
    QStringList getChannelMNames() const;
    struct ChannelData {
        QString m_alias;
        QString m_unit;
        int m_dspChannel;
    };
    const ChannelData getChannelData(QString channelName);
signals:
    void sigScanFinished(bool ok);
    void sigChannelChanged(QString channelMName);

protected:
    QMap<QString, ChannelData> m_channels;

private slots:
    void onTasksFinish(bool ok);
private:
    void createTasks(Zera::ProxyClientPtr pcbClient);
    void notifyError(QString errMsg);
    TaskTemplatePtr getChannelsReadTasks(Zera::PcbInterfacePtr pcbInterface);
    TaskContainerInterfacePtr m_currentTasks;
    QStringList m_tempChannelMNames;
};

#endif // PCBCHANNELMANAGER_H
