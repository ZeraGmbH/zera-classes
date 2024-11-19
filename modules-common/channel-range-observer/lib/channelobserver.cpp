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
    m_reReadTask = TaskContainerSequence::create();
    m_reReadTask = addRangesFetchTasks(std::move(m_reReadTask), channelMName, pcbInterface);
    m_reReadTask->start();
}

TaskContainerInterfacePtr ChannelObserver::addRangesFetchTasks(TaskContainerInterfacePtr tasks,
                                                               const QString &channelMName,
                                                               Zera::PcbInterfacePtr pcbInterface)
{
    tasks->addSub(getReadRangeNamesTask(channelMName, pcbInterface));
    tasks = addReadRangeDetailsTasks(std::move(tasks), channelMName, pcbInterface);
    tasks->addSub(getReadRangeFinalTask(channelMName));
    return tasks;
}

TaskTemplatePtr ChannelObserver::getReadRangeNamesTask(const QString &channelMName, Zera::PcbInterfacePtr pcbInterface)
{
    return TaskChannelGetRangeList::create(
        pcbInterface, channelMName, m_tempRangesNames,
        TRANSACTION_TIMEOUT, [&]{ notifyError(QString("Could not read range list for channel %1").arg(channelMName)); });
}

TaskContainerInterfacePtr ChannelObserver::addReadRangeDetailsTasks(TaskContainerInterfacePtr tasks,
                                                                    const QString &channelMName,
                                                                    Zera::PcbInterfacePtr pcbInterface)
{
    TaskContainerInterfacePtr taskToAdd = TaskContainerParallel::create();

    // TODO: add range tasks

    tasks->addSub(std::move(taskToAdd));
    return tasks;
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
