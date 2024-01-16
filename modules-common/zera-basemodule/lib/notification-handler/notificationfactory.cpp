#include "notificationfactory.h"
#include "notificationcontainer.h"
#include <memory>

NotificationFactory::NotificationFactory()
{
}

void NotificationFactory::createNotification(QVariant msg)
{
    NotificationPtr notification;
    QString strMsg = msg.toString();
    notification = std::make_shared<Notification>(strMsg);
    NotificationContainer::getInstance()->addNotification(notification);
}

void NotificationFactory::removeNotification(int id)
{
    NotificationContainer::getInstance()->removeNotification(id);
}
