#ifndef PCBCHANNELDATA_H
#define PCBCHANNELDATA_H

#include <QObject>
#include <memory>

class PcbChannelData : public QObject
{
    Q_OBJECT
public:
    // For now - let's see where this leads us...
    QString m_alias;
    QString m_unit;
    int m_dspChannel;

signals:
    void sigRangesReloaded();
};

typedef std::shared_ptr<PcbChannelData> PcbChannelDataPtr;

#endif // PCBCHANNELDATA_H
