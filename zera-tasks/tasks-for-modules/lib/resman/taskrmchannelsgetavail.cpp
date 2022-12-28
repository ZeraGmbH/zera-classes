#include "taskrmchannelsgetavail.h"
#include "tasktimeoutdecorator.h"

QStringList TaskRmChannelsGetAvail::m_defaultSenseResourcesIgnored = QStringList() << "mmode";

TaskCompositePtr TaskRmChannelsGetAvail::create(Zera::Server::RMInterfacePtr rmInterface,
                                                int timeout,
                                                QStringList &channelSysNameList,
                                                std::function<void ()> additionalErrorHandler,
                                                QStringList senseResourcesIgnored)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmChannelsGetAvail>(
                                                 rmInterface,
                                                 channelSysNameList,
                                                 senseResourcesIgnored),
                                             additionalErrorHandler);
}

TaskRmChannelsGetAvail::TaskRmChannelsGetAvail(Zera::Server::RMInterfacePtr rmInterface,
                                               QStringList &channelSysNameList,
                                               QStringList senseResourcesIgnored) :
    TaskServerTransactionTemplate(rmInterface),
    m_rmInterface(rmInterface),
    m_channelSysNameList(channelSysNameList),
    m_senseResourcesIgnored(senseResourcesIgnored)
{
}

quint32 TaskRmChannelsGetAvail::sendToServer()
{
    return m_rmInterface->getResources("SENSE");
}

bool TaskRmChannelsGetAvail::handleCheckedServerAnswer(QVariant answer)
{
    fillChannelList(answer);
    return true;
}

void TaskRmChannelsGetAvail::fillChannelList(QVariant answer)
{
    QStringList channelList = answer.toString().split(";", Qt::SkipEmptyParts);
    for(const auto &channel : qAsConst(channelList)) {
        QString entryUpper = channel.toUpper();
        bool ignore = false;
        for(const auto &ignoreEntry : qAsConst(m_senseResourcesIgnored)) {
            QString ignoreEntryUpper = ignoreEntry.toUpper();
            if(entryUpper == ignoreEntryUpper) {
                ignore = true;
                break;
            }
        }
        if(!ignore)
            m_channelSysNameList.append(channel);
    }
}

