#include "taskrangegetadcrejection.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskRangeGetAdcRejection::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelMName, QString rangeName,
                                                 std::shared_ptr<double> valueReceived,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetAdcRejection>(
                                                 pcbInterface,
                                                 channelMName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskRangeGetAdcRejection::TaskRangeGetAdcRejection(Zera::PcbInterfacePtr pcbInterface,
                                                   QString channelMName, QString rangeName,
                                                   std::shared_ptr<double> valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelMName(channelMName),
    m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskRangeGetAdcRejection::sendToServer()
{
    return m_pcbInterface->getADCRejection(m_channelMName, m_rangeName);
}

bool TaskRangeGetAdcRejection::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_valueReceived = answer.toDouble(&ok);
    return ok;
}
