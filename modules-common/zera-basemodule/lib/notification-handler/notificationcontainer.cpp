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
    emit sigNotificationAdded(notif->getId(), notif->getMsg());
}

int NotificationContainer::NotificationsListSize()
{
    return m_notifList.size();
}

void NotificationContainer::notifRemoved(int id)
{
    m_notifList.remove(id);
    emit sigNotifRemoved(id);
}

NotificationContainer::NotificationContainer()
{
}
