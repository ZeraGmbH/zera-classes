#include "taskrmchannelsgetavail.h"
#include "taskdecoratortimeout.h"

QStringList TaskRmChannelsGetAvail::m_defaultSenseResourcesIgnored = QStringList() << "mmode";

TaskTemplatePtr TaskRmChannelsGetAvail::create(Zera::RMInterfacePtr rmInterface,
                                                int timeout,
                                                QStringList &channelMNameList,
                                                std::function<void ()> additionalErrorHandler,
                                                QStringList senseResourcesIgnored)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRmChannelsGetAvail>(
                                                 rmInterface,
                                                 channelMNameList,
                                                 senseResourcesIgnored),
                                             additionalErrorHandler);
}

TaskRmChannelsGetAvail::TaskRmChannelsGetAvail(Zera::RMInterfacePtr rmInterface,
                                               QStringList &channelMNameList,
                                               QStringList senseResourcesIgnored) :
    TaskServerTransactionTemplate(rmInterface),
    m_rmInterface(rmInterface),
    m_channelMNameList(channelMNameList),
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
            m_channelMNameList.append(channel);
    }
}

