#ifndef NOTIFICATIONFACTORY_H
#define NOTIFICATIONFACTORY_H

#include "notification.h"
#include <QVariant>

class NotificationFactory
{
public:
    NotificationFactory();
    void createNotification(QVariant msg, bool priority);
};

#endif // NOTIFICATIONFACTORY_H
