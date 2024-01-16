#ifndef NOTIFICATIONCONTAINER_H
#define NOTIFICATIONCONTAINER_H

#include "notification.h"
#include <QObject>
#include <QHash>
#include <timersingleshotqt.h>

class NotificationContainer : public QObject
{
    Q_OBJECT
public:
    static NotificationContainer* getInstance();
    void addNotification(NotificationPtr notif);
    int NotificationsListSize();
    void removeNotification(int id);

signals:
    void sigNotificationAdded(int id, QString msg);
    void sigNotifRemoved(int id);


private:
    NotificationContainer();

    static NotificationContainer *m_instance;
    int m_id;
    QHash<int, QString> m_notifList;
    TimerTemplateQtPtr m_fadeOutTimer;

};

#endif // NOTIFICATIONCONTAINER_H
