#include "taskrangegetovrejection.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRangeGetOvRejection::create(Zera::PcbInterfacePtr pcbInterface,
                                                QString channelMName, QString rangeName,
                                                std::shared_ptr<double> valueReceived,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetOvRejection>(
                                                 pcbInterface,
                                                 channelMName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskRangeGetOvRejection::TaskRangeGetOvRejection(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelMName, QString rangeName,
                                                 std::shared_ptr<double> valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelMName(channelMName),
    m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskRangeGetOvRejection::sendToServer()
{
    return m_pcbInterface->getOVRejection(m_channelMName, m_rangeName);
}

bool TaskRangeGetOvRejection::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_valueReceived = answer.toDouble(&ok);
    return ok;
}
