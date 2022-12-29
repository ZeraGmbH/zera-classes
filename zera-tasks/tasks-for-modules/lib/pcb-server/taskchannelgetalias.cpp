#include "taskchannelgetalias.h"
#include "tasktimeoutdecorator.h"
#include <reply.h>

TaskCompositePtr TaskChannelGetAlias::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                             QString channelName,
                                             QString &valueReceived,
                                             int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetAlias>(
                                                 pcbInterface,
                                                 channelName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetAlias::TaskChannelGetAlias(Zera::Server::PcbInterfacePtr pcbInterface,
                                         QString channelName,
                                         QString &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskChannelGetAlias::sendToServer()
{
    return m_pcbInterface->getAlias(m_channelName);
}

bool TaskChannelGetAlias::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toString();
    return true;
}
