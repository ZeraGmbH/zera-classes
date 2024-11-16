#ifndef PCBCHANNELMANAGER_H
#define PCBCHANNELMANAGER_H

#include <networkconnectioninfo.h>
#include <proxyclient.h>
#include <pcbinterface.h>
#include <taskcontainerinterface.h>
#include <QObject>

class PcbChannelManager : public QObject
{
    Q_OBJECT
public:
    explicit PcbChannelManager();
    void startScan(Zera::ProxyClientPtr pcbClient);
    QStringList getChannelMNames() const;
signals:
    void sigScanFinished();
    void sigChannelChanged(QString channelMName);
protected:
    QStringList m_channelNames;
    TaskContainerInterfacePtr m_currentTasks;
};

#endif // PCBCHANNELMANAGER_H
