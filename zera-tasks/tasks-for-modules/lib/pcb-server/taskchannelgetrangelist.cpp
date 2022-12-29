#include "taskchannelgetrangelist.h"
#include "tasktimeoutdecorator.h"
#include <reply.h>

TaskCompositePtr TaskChannelGetRangeList::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                               QString channelName,
                                               QStringList &targetRangeList,
                                               int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetRangeList>(
                                                 pcbInterface,
                                                 channelName,
                                                 targetRangeList),
                                             additionalErrorHandler);
}

TaskChannelGetRangeList::TaskChannelGetRangeList(Zera::Server::PcbInterfacePtr pcbInterface,
                                             QString channelName,
                                             QStringList &targetRangeList) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_targetRangeList(targetRangeList)
{
}

quint32 TaskChannelGetRangeList::sendToServer()
{
    return m_pcbInterface->getRangeList(m_channelName);
}

bool TaskChannelGetRangeList::handleCheckedServerAnswer(QVariant answer)
{
    m_targetRangeList = answer.toStringList();
    return true;
}
