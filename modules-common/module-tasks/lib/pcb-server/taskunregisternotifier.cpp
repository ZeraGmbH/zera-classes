#include "taskunregisternotifier.h"
#include "taskdecoratortimeout.h"
#include <reply.h>

TaskTemplatePtr TaskUnregisterNotifier::create(const Zera::PcbInterfacePtr &pcbInterface,
                                               int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskUnregisterNotifier>(pcbInterface),
                                             additionalErrorHandler);
}

TaskUnregisterNotifier::TaskUnregisterNotifier(const Zera::PcbInterfacePtr &pcbInterface) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface)
{
}

quint32 TaskUnregisterNotifier::sendToServer()
{
    return m_pcbInterface->unregisterNotifiers();
}

bool TaskUnregisterNotifier::handleCheckedServerAnswer(const QVariant &answer)
{
    Q_UNUSED(answer)
    return true;
}
