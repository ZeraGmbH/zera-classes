#ifndef NONPRIORITYNOTIFICATION_H
#define NONPRIORITYNOTIFICATION_H

#include "notification.h"
#include <timersingleshotqt.h>
#include <QObject>

class NonPriorityNotification : public Notification
{
    Q_OBJECT
public:
    NonPriorityNotification(QString msg);

private slots:
    void onTimeExpired();
private:
    TimerTemplateQtPtr m_fadeOutTimer;

};

#endif // NONPRIORITYNOTIFICATION_H
