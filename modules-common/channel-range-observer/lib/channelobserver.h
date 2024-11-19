#ifndef CHANNELOBSERVER_H
#define CHANNELOBSERVER_H

#include <pcbinterface.h>
#include <taskcontainerinterface.h>
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
    const QStringList getRangeNames() const;
    TaskTemplatePtr getRangesFetchTasks(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface);
    void startReReadRanges(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface);
signals:
    void sigRangeListChanged(QString channelMName);
private:
    TaskTemplatePtr getReadRangeNamesTask(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface);
    TaskTemplatePtr getReadRangeDetailsTasks(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface);
    TaskTemplatePtr getReadRangeFinalTask(const QString &channelMName);
    static void notifyError(QString errMsg);
    QStringList m_tempRangesNames;
    TaskTemplatePtr m_reReadTask;
};

typedef std::shared_ptr<ChannelObserver> ChannelObserverPtr;

#endif // CHANNELOBSERVER_H
