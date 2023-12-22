#include "nonprioritynotification.h"
#include <timerfactoryqt.h>

NonPriorityNotification::NonPriorityNotification(QString msg)
    : Notification(msg)
{
    m_fadeOutTimer = TimerFactoryQt::createSingleShot(500);
    connect(m_fadeOutTimer.get(), &TimerTemplateQt::sigExpired,
            this, &NonPriorityNotification::onTimeExpired);
    m_fadeOutTimer->start();
}

void NonPriorityNotification::onTimeExpired()
{
    emit removeNotif(m_id);
}
