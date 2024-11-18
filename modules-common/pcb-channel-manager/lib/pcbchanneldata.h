#ifndef PCBCHANNELDATA_H
#define PCBCHANNELDATA_H

#include <tasktemplate.h>
#include <QObject>
#include <QStringList>
#include <memory>

class PcbChannelData : public QObject
{
    Q_OBJECT
public:
    // For now - let's see where this leads us...
    QString m_alias;
    QString m_unit;
    int m_dspChannel;
    TaskTemplatePtr m_currentTasks;
    QStringList m_tempChannelRanges;
signals:
    void sigRangesReloaded();
};

typedef std::shared_ptr<PcbChannelData> PcbChannelDataPtr;

#endif // PCBCHANNELDATA_H
