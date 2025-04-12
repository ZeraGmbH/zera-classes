#include "taskchannelgetunit.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskChannelGetUnit::create(Zera::PcbInterfacePtr pcbInterface, QString channelName, std::shared_ptr<QString> valueReceived, int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetUnit>(
                                                 pcbInterface,
                                                 channelName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetUnit::TaskChannelGetUnit(Zera::PcbInterfacePtr pcbInterface,
                                       QString channelName,
                                       std::shared_ptr<QString> valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskChannelGetUnit::sendToServer()
{
    return m_pcbInterface->getUnit(m_channelName);
}

bool TaskChannelGetUnit::handleCheckedServerAnswer(QVariant answer)
{
    *m_valueReceived = answer.toString();
    return true;
}
