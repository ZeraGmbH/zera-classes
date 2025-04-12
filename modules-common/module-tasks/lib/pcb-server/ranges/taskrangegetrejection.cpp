#include "taskrangegetrejection.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRangeGetRejection::create(Zera::PcbInterfacePtr pcbInterface,
                                              QString channelMName, QString rangeName,
                                              std::shared_ptr<double> valueReceived,
                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetRejection>(
                                                 pcbInterface,
                                                 channelMName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskRangeGetRejection::TaskRangeGetRejection(Zera::PcbInterfacePtr pcbInterface,
                                             QString channelMName, QString rangeName,
                                             std::shared_ptr<double> valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelMName(channelMName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskRangeGetRejection::sendToServer()
{
    return m_pcbInterface->getRejection(m_channelMName, m_rangeName);
}

bool TaskRangeGetRejection::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_valueReceived = answer.toDouble(&ok);
    return ok;
}
