#ifndef PCBCHANNELMANAGER_H
#define PCBCHANNELMANAGER_H

#include <proxyclient.h>
#include <taskcontainerinterface.h>

class PcbChannelManager : public QObject
{
    Q_OBJECT
public:
    void startScan(Zera::ProxyClientPtr pcbClient);
    QStringList getChannelMNames() const;
signals:
    void sigScanFinished(bool ok);
    void sigChannelChanged(QString channelMName);

protected:
    QStringList m_channelMNames;

private slots:
    void onTasksFinish(bool ok);
private:
    void createTasks(Zera::ProxyClientPtr pcbClient);
    TaskContainerInterfacePtr m_currentTasks;
};

#endif // PCBCHANNELMANAGER_H
