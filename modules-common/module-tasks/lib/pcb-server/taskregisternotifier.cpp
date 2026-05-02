#include "taskregisternotifier.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRegisterNotifier::create(const Zera::PcbInterfacePtr &pcbInterface,
                                             const QString &scpiQuery,
                                             int notificationId,
                                             int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRegisterNotifier>(
                                                 pcbInterface,
                                                 scpiQuery,
                                                 notificationId),
                                             additionalErrorHandler);

}

TaskRegisterNotifier::TaskRegisterNotifier(const Zera::PcbInterfacePtr &pcbInterface,
                                           const QString &scpiQuery,
                                           int notificationId) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_scpiQuery(scpiQuery),
    m_notificationId(notificationId)
{
    Q_ASSERT(scpiQuery.endsWith("?"));
}

quint32 TaskRegisterNotifier::sendToServer()
{
    return m_pcbInterface->registerNotifier(m_scpiQuery, m_notificationId);
}

bool TaskRegisterNotifier::handleCheckedServerAnswer(const QVariant &answer)
{
    Q_UNUSED(answer)
    return true;
}

