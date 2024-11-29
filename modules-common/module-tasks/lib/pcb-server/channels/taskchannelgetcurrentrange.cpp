#include "taskchannelgetcurrentrange.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskChannelGetCurrentRange::create(Zera::PcbInterfacePtr pcbInterface, QString channelName,
                                                   QString &valueReceived,
                                                   int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetCurrentRange>(
                                                 pcbInterface,
                                                 channelName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetCurrentRange::TaskChannelGetCurrentRange(Zera::PcbInterfacePtr pcbInterface,
                                                       QString channelName,
                                                       QString &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskChannelGetCurrentRange::sendToServer()
{
    return m_pcbInterface->getRange(m_channelName);
}

bool TaskChannelGetCurrentRange::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toString();
    return true;
}
