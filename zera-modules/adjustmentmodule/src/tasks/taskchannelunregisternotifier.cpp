#include "taskchannelunregisternotifier.h"
#include "adjustmentmodulemeasprogram.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

std::unique_ptr<TaskComposite> TaskChannelUnregisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface)
{
    return std::make_unique<TaskChannelUnregisterNotifier>(pcbInterface);
}

std::unique_ptr<TaskComposite> TaskChannelUnregisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                                     int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(pcbInterface), additionalErrorHandler);
}

TaskChannelUnregisterNotifier::TaskChannelUnregisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface) :
    m_pcbInterface(pcbInterface)
{
}

void TaskChannelUnregisterNotifier::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskChannelUnregisterNotifier::onRmAnswer);
    m_msgnr = m_pcbInterface->unregisterNotifiers();
}

void TaskChannelUnregisterNotifier::onRmAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if(msgnr == m_msgnr) {
        finishTask(reply == ack);
    }
}
