#include "taskchannelgetrejection.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

TaskCompositePtr TaskChannelGetRejection::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &valueReceived,
                                                 int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetRejection>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetRejection::TaskChannelGetRejection(Zera::Server::PcbInterfacePtr pcbInterface,
                                                 QString channelSysName, QString rangeName,
                                                 double &valueReceived) :
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName),
    m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

void TaskChannelGetRejection::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelGetRejection::onRmAnswer);
    m_msgnr = m_pcbInterface->getRejection(m_channelSysName, m_rangeName);
}

void TaskChannelGetRejection::onRmAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            m_valueReceived = answer.toDouble();
        finishTask(reply == ack);
    }
}
