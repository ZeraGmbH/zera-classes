#include "taskchannelgetrangelist.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

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
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_targetRangeList(targetRangeList)
{
}

void TaskChannelGetRangeList::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer,
            this, &TaskChannelGetRangeList::onServerAnswer);
    m_msgnr = m_pcbInterface->getRangeList(m_channelName);
}

void TaskChannelGetRangeList::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(msgnr == m_msgnr) {
        if (reply == ack)
            m_targetRangeList = answer.toStringList();
        finishTask(reply == ack);
    }
}
