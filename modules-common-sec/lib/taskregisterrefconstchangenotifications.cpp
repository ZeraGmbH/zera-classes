#include "taskregisterrefconstchangenotifications.h"
#include "taskregisternotifier.h"

std::unique_ptr<TaskRegisterRefConstChangeNotifications> TaskRegisterRefConstChangeNotifications::create(Zera::PcbInterfacePtr pcbInterface,
                                                                                                         QStringList refInputs,
                                                                                                         int firstNotificationId,
                                                                                                         std::function<void ()> additionalErrorHandler)
{
    std::unique_ptr<TaskRegisterRefConstChangeNotifications> task = std::make_unique<TaskRegisterRefConstChangeNotifications>();
    int currentNotificationId = firstNotificationId;
    for(const auto &refInput : refInputs) {
        task->addSub(TaskRegisterNotifier::create(pcbInterface,
                                                  QString("SOURCE:%1:CONSTANT?").arg(refInput),
                                                  currentNotificationId,
                                                  TRANSACTION_TIMEOUT,
                                                  additionalErrorHandler));
        task->addNotificationId(currentNotificationId);
        currentNotificationId++;
    }
    return task;
}

QList<int> TaskRegisterRefConstChangeNotifications::getnotificationIds()
{
    return m_notificationIds;
}

void TaskRegisterRefConstChangeNotifications::addNotificationId(int id)
{
    m_notificationIds.append(id);
}
