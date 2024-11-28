#include "taskrangegetisavailable.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRangeGetIsAvailable::create(Zera::PcbInterfacePtr pcbInterface,
                                                QString channelSysName, QString rangeName,
                                                bool &valueReceived,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetIsAvailable>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);

}

TaskRangeGetIsAvailable::TaskRangeGetIsAvailable(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName,
                                                 QString rangeName,
                                                 bool &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskRangeGetIsAvailable::sendToServer()
{
    return m_pcbInterface->isAvail(m_channelSysName, m_rangeName);
}

bool TaskRangeGetIsAvailable::handleCheckedServerAnswer(QVariant answer)
{
    m_valueReceived = answer.toBool();
    return true;
}
