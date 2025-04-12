#include "taskchannelgetavail.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskChannelGetAvail::create(Zera::PcbInterfacePtr pcbInterface,
                                            std::shared_ptr<QStringList> channelsReceived,
                                            int timeout,
                                            std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetAvail>(
                                                 pcbInterface,
                                                 channelsReceived),
                                             additionalErrorHandler);
}

TaskChannelGetAvail::TaskChannelGetAvail(Zera::PcbInterfacePtr pcbInterface, std::shared_ptr<QStringList> channelsReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_channelsReceived(channelsReceived)
{
}

quint32 TaskChannelGetAvail::sendToServer()
{
    return m_pcbInterface->getChannelList();
}

bool TaskChannelGetAvail::handleCheckedServerAnswer(QVariant answer)
{
    *m_channelsReceived = answer.toStringList();
    return true;
}
