#include "taskunregisternotifier.h"
#include "tasktimeoutdecorator.h"
#include "reply.h"

std::unique_ptr<TaskComposite> TaskUnregisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface)
{
    return std::make_unique<TaskUnregisterNotifier>(pcbInterface);
}

std::unique_ptr<TaskComposite> TaskUnregisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                              int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout, create(pcbInterface), additionalErrorHandler);
}

TaskUnregisterNotifier::TaskUnregisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface) :
    m_pcbInterface(pcbInterface)
{
}

void TaskUnregisterNotifier::start()
{
    connect(m_pcbInterface.get(), &Zera::Server::cPCBInterface::serverAnswer, this, &TaskUnregisterNotifier::onRmAnswer);
    m_msgnr = m_pcbInterface->unregisterNotifiers();
}

void TaskUnregisterNotifier::onRmAnswer(quint32 msgnr, quint8 reply, QVariant)
{
    if(msgnr == m_msgnr)
        finishTask(reply == ack);
}
