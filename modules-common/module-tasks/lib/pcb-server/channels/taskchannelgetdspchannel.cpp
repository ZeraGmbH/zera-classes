#include "taskchannelgetdspchannel.h"
#include <taskdecoratortimeout.h>

TaskTemplatePtr TaskChannelGetDspChannel::create(Zera::PcbInterfacePtr pcbInterface,
                                                 QString channelName,
                                                 std::shared_ptr<int> valueReceived,
                                                 int timeout,
                                                 std::function<void()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetDspChannel>(
                                                 pcbInterface,
                                                 channelName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetDspChannel::TaskChannelGetDspChannel(Zera::PcbInterfacePtr pcbInterface,
                                                   QString channelName,
                                                   std::shared_ptr<int> valueReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelName(channelName),
    m_valueReceived(valueReceived)
{
}

quint32 TaskChannelGetDspChannel::sendToServer()
{
    return m_pcbInterface->getDSPChannel(m_channelName);
}

bool TaskChannelGetDspChannel::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_valueReceived = answer.toInt(&ok);
    return ok;
}
