#include "taskrangegeturvalue.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRangeGetUrValue::create(Zera::PcbInterfacePtr pcbInterface,
                                               QString channelSysName, QString rangeName,
                                               double &valueReceived,
                                               int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetUrValue>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskRangeGetUrValue::TaskRangeGetUrValue(Zera::PcbInterfacePtr pcbInterface,
                                             QString channelSysName, QString rangeName,
                                             double &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskRangeGetUrValue::sendToServer()
{
    return m_pcbInterface->getUrvalue(m_channelSysName, m_rangeName);
}

bool TaskRangeGetUrValue::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toDouble();
    return true;
}
