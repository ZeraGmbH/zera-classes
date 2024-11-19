#include "channelobserver.h"
#include <taskchannelgetrangelist.h>
#include <taskcontainersequence.h>
#include <taskcontainerparallel.h>
#include <tasklambdarunner.h>

const QStringList ChannelObserver::getRangeNames() const
{
    return m_tempRangesNames;
}

void ChannelObserver::startReReadRanges(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface)
{
    m_reReadTask = getRangesFetchTasks(channelMName, pcbInterface);
    m_reReadTask->start();
}

TaskTemplatePtr ChannelObserver::getRangesFetchTasks(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface)
{
    TaskContainerInterfacePtr rangeTasks = TaskContainerSequence::create();
    rangeTasks->addSub(getReadRangeNamesTask(channelMName, pcbInterface));
    rangeTasks->addSub(getReadRangeDetailsTasks(channelMName, pcbInterface));
    rangeTasks->addSub(getReadRangeFinalTask(channelMName));
    return rangeTasks;
}

TaskTemplatePtr ChannelObserver::getReadRangeNamesTask(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface)
{
    return TaskChannelGetRangeList::create(
        pcbInterface, channelMName, m_tempRangesNames,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(channelMName)); });
}

TaskTemplatePtr ChannelObserver::getReadRangeDetailsTasks(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface)
{
    TaskContainerInterfacePtr allChannelsTasks = TaskContainerParallel::create();

    // TODO: add range tasks

    return allChannelsTasks;
}

TaskTemplatePtr ChannelObserver::getReadRangeFinalTask(const QString &channelMName)
{
    return TaskLambdaRunner::create([=]() {
        emit sigRangeListChanged(channelMName);
        return true;
    });
}

void ChannelObserver::notifyError(QString errMsg)
{
    qWarning("ChannelObserver error: %s", qPrintable(errMsg));
}
