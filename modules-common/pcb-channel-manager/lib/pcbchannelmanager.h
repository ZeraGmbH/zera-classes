#ifndef PCBCHANNELMANAGER_H
#define PCBCHANNELMANAGER_H

#include <QObject>

namespace ZeraModules {
    class ModuleManager;
}

class PcbChannelManager : public QObject
{
    Q_OBJECT
public:
    explicit PcbChannelManager();
    void startScan();
    QStringList getChannelMNames() const;
signals:
    void sigScanFinished();
    void sigChannelChanged(QString channelMName);
private:
    // not for modules use
    friend class ZeraModules::ModuleManager;
    void clear();

    QStringList m_channelNames;
};

#endif // PCBCHANNELMANAGER_H
