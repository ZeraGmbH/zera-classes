#include "taskchannelregisternotifier.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

std::unique_ptr<TaskComposite> TaskChannelRegisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName)
{
    return std::make_unique<TaskChannelRegisterNotifier>(pcbInterface, channelName);
}

std::unique_ptr<TaskComposite> TaskChannelRegisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName,
                                                                   int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(pcbInterface, channelName), additionalErrorHandler);
}

TaskChannelRegisterNotifier::TaskChannelRegisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface, QString channelName) :
    m_pcbInterface(pcbInterface),
    m_channelName(channelName)
{
}

void TaskChannelRegisterNotifier::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelRegisterNotifier::onRmAnswer);
    m_msgnr = m_pcbInterface->registerNotifier(QString("SENS:%1:RANG:CAT?").arg(m_channelName), "1");
}

void TaskChannelRegisterNotifier::onRmAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if(msgnr == m_msgnr) {
        finishTask(reply == ack);
    }
}

