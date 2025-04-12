#ifndef TASKREFPOWERREGISTERCONSTCHANGENOTIFICATIONS_H
#define TASKREFPOWERREGISTERCONSTCHANGENOTIFICATIONS_H

#include <taskcontainerparallel.h>
#include <pcbinterface.h>

class TaskRefPowerRegisterConstChangeNotifications : public TaskContainerParallel
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRefPowerRegisterConstChangeNotifications> create(Zera::PcbInterfacePtr pcbInterface,
                                                                                QStringList inputNames,
                                                                                int firstNotificationId,
                                                                                std::function<void()> additionalErrorHandler = []{});
    QMap<int /* notifyId */, QString /* refInputName */> getNotificationIds() const;
private:
    void addNotificationId(int id, QString inputName);
    QMap<int /* notifyId */, QString /* refInputName */> m_notificationIds;
};

typedef std::unique_ptr<TaskRefPowerRegisterConstChangeNotifications> TaskRegisterRefConstChangeNotificationsPtr;

#endif // TASKREFPOWERREGISTERCONSTCHANGENOTIFICATIONS_H
