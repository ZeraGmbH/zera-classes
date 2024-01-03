#include "notificationfactory.h"
#include "nonprioritynotification.h"
#include "notificationcontainer.h"
#include <memory>

NotificationFactory::NotificationFactory()
{
}

void NotificationFactory::createNotification(QVariant msg, bool priority)
{
    NotificationPtr notification;
    QString strMsg = msg.toString();
    if(priority)
        notification = std::make_shared<Notification>(strMsg);
    else
        notification = std::make_shared<NonPriorityNotification>(strMsg);

    NotificationContainer::getInstance()->addNotification(notification);
}
