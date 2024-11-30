#include "taskchannelgetalias.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskChannelGetAlias::create(Zera::PcbInterfacePtr pcbInterface,
                                            QString channelName,
                                            QString &valueReceived,
                                            int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetAlias>(
                                                 pcbInterface,
                                                 channelName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetAlias::TaskChannelGetAlias(Zera::PcbInterfacePtr pcbInterface,
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
