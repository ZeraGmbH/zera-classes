#ifndef TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H
#define TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H

#include <taskcontainerparallel.h>
#include <pcbinterface.h>

class TaskRegisterRefConstChangeNotifications : public TaskContainerParallel
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRegisterRefConstChangeNotifications> create(Zera::PcbInterfacePtr pcbInterface,
                                                                           QStringList inputNames,
                                                                           int firstNotificationId,
                                                                           std::function<void()> additionalErrorHandler = []{});
    QMap<int /* notifyId */, QString /* refInputName */> getNotificationIds() const;
private:
    void addNotificationId(int id, QString inputName);
    QMap<int /* notifyId */, QString /* refInputName */> m_notificationIds;
};

typedef std::unique_ptr<TaskRegisterRefConstChangeNotifications> TaskRegisterRefConstChangeNotificationsPtr;

#endif // TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H
