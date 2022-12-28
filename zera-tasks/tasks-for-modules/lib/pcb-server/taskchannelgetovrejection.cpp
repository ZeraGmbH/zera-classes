#include "taskchannelgetovrejection.h"
#include "tasktimeoutdecorator.h"
#include <reply.h>

TaskCompositePtr TaskChannelGetOvRejection::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                   QString channelSysName, QString rangeName,
                                                   double &valueReceived,
                                                   int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskChannelGetOvRejection>(
                                                 pcbInterface,
                                                 channelSysName, rangeName,
                                                 valueReceived),
                                             additionalErrorHandler);
}

TaskChannelGetOvRejection::TaskChannelGetOvRejection(Zera::Server::PcbInterfacePtr pcbInterface,
                                                     QString channelSysName, QString rangeName,
                                                     double &valueReceived) :
    m_pcbInterface(pcbInterface),
    m_channelSysName(channelSysName), m_rangeName(rangeName),
    m_valueReceived(valueReceived)
{
}

void TaskChannelGetOvRejection::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer,
            this, &TaskChannelGetOvRejection::onServerAnswer);
    m_msgnr = m_pcbInterface->getOVRejection(m_channelSysName, m_rangeName);
}

void TaskChannelGetOvRejection::onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(m_msgnr == msgnr) {
        if (reply == ack)
            m_valueReceived = answer.toDouble();
        finishTask(reply == ack);
    }
}
