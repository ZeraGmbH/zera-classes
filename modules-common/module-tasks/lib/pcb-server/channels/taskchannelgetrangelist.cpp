#include "taskchannelgetrangelist.h"
#include "taskdecoratortimeout.h"
#include <reply.h>

TaskTemplatePtr TaskChannelGetRangeList::create(Zera::PcbInterfacePtr pcbInterface,
                                                QString channelName,
                                                std::shared_ptr<QStringList> rangeList,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetRangeList>(
                                                 pcbInterface,
                                                 channelName,
                                                 rangeList),
                                             additionalErrorHandler);
}

TaskChannelGetRangeList::TaskChannelGetRangeList(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelName,
                                                 std::shared_ptr<QStringList> rangeList) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_rangeList(rangeList)
{
}

quint32 TaskChannelGetRangeList::sendToServer()
{
    return m_pcbInterface->getRangeList(m_channelName);
}

bool TaskChannelGetRangeList::handleCheckedServerAnswer(QVariant answer)
{
    *m_rangeList = answer.toStringList();
    return true;
}
