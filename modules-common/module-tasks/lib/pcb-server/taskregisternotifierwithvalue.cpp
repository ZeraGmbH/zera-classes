#include "taskregisternotifierwithvalue.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRegisterNotifierWithValue::create(Zera::PcbInterfacePtr pcbInterface,
                                             QString scpiQuery,
                                             int notificationId,
                                             int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRegisterNotifierWithValue>(
                                                 pcbInterface,
                                                 scpiQuery,
                                                 notificationId),
                                             additionalErrorHandler);

}

TaskRegisterNotifierWithValue::TaskRegisterNotifierWithValue(Zera::PcbInterfacePtr pcbInterface, QString scpiQuery, int notificationId) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_scpiQuery(scpiQuery),
    m_notificationId(notificationId)
{
    Q_ASSERT(scpiQuery.endsWith("?"));
}

quint32 TaskRegisterNotifierWithValue::sendToServer()
{
    return m_pcbInterface->registerNotifier(m_scpiQuery, m_notificationId, true);
}

bool TaskRegisterNotifierWithValue::handleCheckedServerAnswer(QVariant answer)
{
    Q_UNUSED(answer)
    return true;
}

