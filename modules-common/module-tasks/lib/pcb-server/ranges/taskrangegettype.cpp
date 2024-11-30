#include "taskrangegettype.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskRangeGetType::create(Zera::PcbInterfacePtr pcbInterface,
                                         QString channelMName, QString rangeName,
                                         int &valueReceived,
                                         int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRangeGetType>(
                                                 pcbInterface,
                                                 channelMName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskRangeGetType::TaskRangeGetType(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelMName, QString rangeName,
                                   int &valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelMName(channelMName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)

{
}

quint32 TaskRangeGetType::sendToServer()
{
    return m_pcbInterface->getType(m_channelMName, m_rangeName);
}

bool TaskRangeGetType::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    m_valueReceived = answer.toInt(&ok);
    return ok;
}
