#include "taskchannelreadranges.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

std::unique_ptr<TaskComposite> TaskChannelReadRanges::create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                             QStringList &targetRangeList)
{
    return std::make_unique<TaskChannelReadRanges>(pcbInterface, channelName, targetRangeList);
}

std::unique_ptr<TaskComposite> TaskChannelReadRanges::create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                             QStringList &targetRangeList,
                                                             int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(pcbInterface, channelName, targetRangeList), additionalErrorHandler);
}

TaskChannelReadRanges::TaskChannelReadRanges(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName, QStringList &targetRangeList) :
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_targetRangeList(targetRangeList)
{
}

void TaskChannelReadRanges::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelReadRanges::onRmAnswer);
    m_msgnr = m_pcbInterface->getRangeList(m_channelName);
}

void TaskChannelReadRanges::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(msgnr == m_msgnr) {
        if (reply == ack)
            m_targetRangeList = answer.toStringList();
        finishTask(reply == ack);
    }
}
