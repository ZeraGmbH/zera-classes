#include "taskregisterrefconstchangenotifications.h"
#include "taskregisternotifier.h"

std::unique_ptr<TaskRegisterRefConstChangeNotifications> TaskRegisterRefConstChangeNotifications::create(Zera::PcbInterfacePtr pcbInterface,
                                                                                                         QStringList inputNames,
                                                                                                         int firstNotificationId,
                                                                                                         std::function<void ()> additionalErrorHandler)
{
    std::unique_ptr<TaskRegisterRefConstChangeNotifications> task = std::make_unique<TaskRegisterRefConstChangeNotifications>();
    int currentNotificationId = firstNotificationId;
    for(const auto &refInput : inputNames) {
        task->addSub(TaskRegisterNotifier::create(pcbInterface,
                                                  QString("SOURCE:%1:CONSTANT?").arg(refInput),
                                                  currentNotificationId,
                                                  TRANSACTION_TIMEOUT,
                                                  additionalErrorHandler));
        task->addNotificationId(currentNotificationId, refInput);
        currentNotificationId++;
    }
    return task;
}

QMap<int, QString> TaskRegisterRefConstChangeNotifications::getNotificationIds() const
{
    return m_notificationIds;
}

void TaskRegisterRefConstChangeNotifications::addNotificationId(int id, QString inputName)
{
    Q_ASSERT(!m_notificationIds.contains(id));
    m_notificationIds[id] = inputName;
}
