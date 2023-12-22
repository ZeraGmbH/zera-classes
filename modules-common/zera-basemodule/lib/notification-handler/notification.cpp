#include "notification.h"

static int currentId = 0;

Notification::Notification(QString msg) :
    m_msg(msg),
    m_id(currentId++)
{
}

int Notification::getId()
{
    return m_id;
}

QString Notification::getMsg()
{
    return m_msg;
}
