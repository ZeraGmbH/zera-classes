#ifndef CHANNELOBSERVER_H
#define CHANNELOBSERVER_H

#include <tasktemplate.h>
#include <QObject>
#include <QStringList>
#include <memory>

class ChannelObserver : public QObject
{
    Q_OBJECT
public:
    // For now - let's see where this leads us...
    QString m_alias;
    QString m_unit;
    int m_dspChannel;
    TaskTemplatePtr m_currentTasks;
    QStringList m_tempChannelRanges;
};

typedef std::shared_ptr<ChannelObserver> ChannelObserverPtr;

#endif // CHANNELOBSERVER_H
