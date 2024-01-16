#ifndef NOTIFICATIONFACTORY_H
#define NOTIFICATIONFACTORY_H

#include "notification.h"
#include <QVariant>

class NotificationFactory
{
public:
    NotificationFactory();
    void createNotification(QVariant msg);
    void removeNotification(int id);
};

#endif // NOTIFICATIONFACTORY_H
