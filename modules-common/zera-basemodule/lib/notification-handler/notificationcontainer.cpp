#include "notificationcontainer.h"
#include <timerfactoryqt.h>

NotificationContainer *NotificationContainer::m_instance = nullptr;

NotificationContainer *NotificationContainer::getInstance()
{
    if(!m_instance)
        m_instance = new NotificationContainer();
    return m_instance;
}

void NotificationContainer::addNotification(NotificationPtr notif)
{
    m_notifList.insert(notif->getId(), notif->getMsg());
    connect(notif.get(), &Notification::removeNotif,
            this, &NotificationContainer::onNotifRemoved);
    emit notificationAdded();
}

void NotificationContainer::removeNotificationMsg(NotificationPtr notif)
{
    m_notifList.remove(notif->getId());
}

int NotificationContainer::NotificationsListSize()
{
    return m_notifList.size();
}

void NotificationContainer::onNotifRemoved(int id)
{
    m_notifList.remove(id);
}

NotificationContainer::NotificationContainer()
{
}
