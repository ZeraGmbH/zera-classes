#include "taskchannelgeturvalue.h"
#include "tasktimeoutdecorator.h"

TaskCompositePtr TaskChannelGetUrValue::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                               QString channelSysName, QString rangeName,
                                               double &valueReceived,
                                               int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetUrValue>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetUrValue::TaskChannelGetUrValue(Zera::Server::PcbInterfacePtr pcbInterface,
                                             QString channelSysName, QString rangeName,
                                             double &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskChannelGetUrValue::sendToServer()
{
    return m_pcbInterface->getUrvalue(m_channelSysName, m_rangeName);
}

bool TaskChannelGetUrValue::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toDouble();
    return true;
}
