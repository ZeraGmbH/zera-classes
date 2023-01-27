#include "taskchannelgetrejection.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskChannelGetRejection::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &valueReceived,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetRejection>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetRejection::TaskChannelGetRejection(Zera::Server::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskChannelGetRejection::sendToServer()
{
    return m_pcbInterface->getRejection(m_channelSysName, m_rangeName);
}

bool TaskChannelGetRejection::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toDouble();
    return true;
}
