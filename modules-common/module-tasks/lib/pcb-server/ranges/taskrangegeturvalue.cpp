#include "taskrangegeturvalue.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRangeGetUrValue::create(Zera::PcbInterfacePtr pcbInterface,
                                            QString channelMName, QString rangeName,
                                            std::shared_ptr<double> valueReceived,
                                            int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetUrValue>(
                                                 pcbInterface,
                                                 channelMName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskRangeGetUrValue::TaskRangeGetUrValue(Zera::PcbInterfacePtr pcbInterface,
                                         QString channelMName, QString rangeName,
                                         std::shared_ptr<double> valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelMName(channelMName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskRangeGetUrValue::sendToServer()
{
    return m_pcbInterface->getUrvalue(m_channelMName, m_rangeName);
}

bool TaskRangeGetUrValue::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_valueReceived = answer.toDouble(&ok);
    return ok;
}
