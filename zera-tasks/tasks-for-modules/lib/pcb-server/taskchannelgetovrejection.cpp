#include "taskchannelgetovrejection.h"
#include "tasktimeoutdecorator.h"
#include <reply.h>

TaskCompositePtr TaskChannelGetOvRejection::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                   QString channelSysName, QString rangeName,
                                                   double &valueReceived,
                                                   int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetOvRejection>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetOvRejection::TaskChannelGetOvRejection(Zera::Server::PcbInterfacePtr pcbInterface,
                                                     QString channelSysName, QString rangeName,
                                                     double &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskChannelGetOvRejection::sendToServer()
{
    return m_pcbInterface->getOVRejection(m_channelSysName, m_rangeName);
}

bool TaskChannelGetOvRejection::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toDouble();
    return true;
}
