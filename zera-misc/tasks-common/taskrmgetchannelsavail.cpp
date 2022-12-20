#include "taskrmgetchannelsavail.h"
#include "tasktimeoutdecorator.h"

QStringList TaskRmGetChannelsAvail::m_defaultSenseResourcesIgnored = QStringList() << "mmode";

TaskCompositePtr TaskRmGetChannelsAvail::create(AbstractRmInterfacePtr rmInterface,
                                                int timeout,
                                                QStringList &channelSysNameList,
                                                std::function<void ()> additionalErrorHandler,
                                                QStringList senseResourcesIgnored)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskRmGetChannelsAvail>(
                                                 rmInterface,
                                                 channelSysNameList,
                                                 senseResourcesIgnored),
                                             additionalErrorHandler);
}

TaskRmGetChannelsAvail::TaskRmGetChannelsAvail(AbstractRmInterfacePtr rmInterface,
                                               QStringList &channelSysNameList,
                                               QStringList senseResourcesIgnored) :
    TaskServerTransactionTemplate(rmInterface),
    m_rmInterface(rmInterface),
    m_channelSysNameList(channelSysNameList),
    m_senseResourcesIgnored(senseResourcesIgnored)
{
}

quint32 TaskRmGetChannelsAvail::sendToServer()
{
    return m_rmInterface->getResources("SENSE");
}

bool TaskRmGetChannelsAvail::handleServerAnswer(QVariant answer)
{
    fillChannelList(answer);
    return true;
}

void TaskRmGetChannelsAvail::fillChannelList(QVariant answer)
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

