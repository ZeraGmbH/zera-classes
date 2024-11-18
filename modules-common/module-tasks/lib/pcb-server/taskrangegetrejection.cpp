#include "taskrangegetrejection.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRangeGetRejection::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &valueReceived,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetRejection>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskRangeGetRejection::TaskRangeGetRejection(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskRangeGetRejection::sendToServer()
{
    return m_pcbInterface->getRejection(m_channelSysName, m_rangeName);
}

bool TaskRangeGetRejection::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toDouble();
    return true;
}
