#ifndef PCBCHANNELMANAGER_H
#define PCBCHANNELMANAGER_H

#include <QObject>

class PcbChannelManager : public QObject
{
    Q_OBJECT
public:
    explicit PcbChannelManager();
    void startScan();
    QStringList getChannelMNames() const;
    void clear();
signals:
    void sigChannelsAvailable();
    void sigChannelChanged(QString channelMName);
private:
};

#endif // PCBCHANNELMANAGER_H
