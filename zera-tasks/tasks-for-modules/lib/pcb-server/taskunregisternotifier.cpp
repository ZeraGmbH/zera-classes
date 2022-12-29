#include "taskunregisternotifier.h"
#include "tasktimeoutdecorator.h"
#include <reply.h>

TaskCompositePtr TaskUnregisterNotifier::create(Zera::Server::PcbInterfacePtr pcbInterface,
                                                int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskTimeoutDecorator::wrapTimeout(timeout,
                                             std::make_unique<TaskUnregisterNotifier>(pcbInterface),
                                             additionalErrorHandler);
}

TaskUnregisterNotifier::TaskUnregisterNotifier(Zera::Server::PcbInterfacePtr pcbInterface) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface)
{
}

quint32 TaskUnregisterNotifier::sendToServer()
{
    return m_pcbInterface->unregisterNotifiers();
}

bool TaskUnregisterNotifier::handleCheckedServerAnswer(QVariant answer)
{
    return true;
}
