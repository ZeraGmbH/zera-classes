#include "taskchannelreadalias.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskChannelReadAlias::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                              QString channelName,
                                              QString &valueReceived,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelReadAlias>(
                                                 pcbInterface,
                                                 channelName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelReadAlias::TaskChannelReadAlias(Zera::Server::PcbInterfacePtr pcbInterface,
                                           QString channelName,
                                           QString &valueReceived) :
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_valueReceived(valueReceived)
{
}

void TaskChannelReadAlias::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelReadAlias::onRmAnswer);
    m_msgnr = m_pcbInterface->getAlias(m_channelName);
}

void TaskChannelReadAlias::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            m_valueReceived = answer.toString();
        finishTask(reply == ack);
    }
}
