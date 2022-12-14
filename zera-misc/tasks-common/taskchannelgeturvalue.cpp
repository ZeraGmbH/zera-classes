#include "taskchannelgeturvalue.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskChannelGetUrValue::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                               QString channelSysName, QString rangeName,
                                               double &valueReceived,
                                               int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetUrValue>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetUrValue::TaskChannelGetUrValue(Zera::Server::PcbInterfacePtr pcbInterface,
                                             QString channelSysName, QString rangeName,
                                             double &valueReceived) :
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName),
    m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

void TaskChannelGetUrValue::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelGetUrValue::onServerAnswer);
    m_msgnr = m_pcbInterface->getUrvalue(m_channelSysName, m_rangeName);
}

void TaskChannelGetUrValue::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            m_valueReceived = answer.toDouble();
        finishTask(reply == ack);
    }
}
