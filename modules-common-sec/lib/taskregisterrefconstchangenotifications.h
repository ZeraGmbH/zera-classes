#ifndef TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H
#define TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H

#include <taskcontainerparallel.h>
#include <pcbinterface.h>

class TaskRegisterRefConstChangeNotifications : public TaskContainerParallel
{
    Q_OBJECT
public:
    static std::unique_ptr<TaskRegisterRefConstChangeNotifications> create(Zera::PcbInterfacePtr pcbInterface,
                                                                           QStringList refInputs,
                                                                           int firstNotificationId,
                                                                           std::function<void()> additionalErrorHandler = []{});
    QList<int> getnotificationIds();
private:
    void addNotificationId(int id);
    QList<int> m_notificationIds;
};

#endif // TASKREGISTERREFCONSTCHANGENOTIFICATIONS_H
