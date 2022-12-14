#include "taskchannelgeturvalue.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

std::unique_ptr<TaskComposite> TaskChannelGetUrValue::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                             QString channelSysName, QString rangeName,
                                                             double &urValue)
{
    return std::make_unique<TaskChannelGetUrValue>(pcbInterface,
                                                   channelSysName, rangeName, urValue);
}

std::unique_ptr<TaskComposite> TaskChannelGetUrValue::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                             QString channelSysName, QString rangeName,
                                                             double &urValue,
                                                             int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(pcbInterface,
                                                             channelSysName, rangeName,
                                                             urValue),
                                             additionalErrorHandler);
}

TaskChannelGetUrValue::TaskChannelGetUrValue(Zera::Server::PcbInterfacePtr pcbInterface, QString channelSysName, QString rangeName, double &urValue) :
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName),
    m_rangeName(rangeName),
    m_urValue(urValue)
{
}

void TaskChannelGetUrValue::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelGetUrValue::onRmAnswer);
    m_msgnr = m_pcbInterface->getUrvalue(m_channelSysName, m_rangeName);
}

void TaskChannelGetUrValue::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            m_urValue = answer.toDouble();
        finishTask(reply == ack);
    }
}
