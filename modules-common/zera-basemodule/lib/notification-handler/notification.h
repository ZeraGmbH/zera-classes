#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <memory>

class Notification : public QObject
{
    Q_OBJECT
public:
    Notification(QString msg);
    int getId();
    QString getMsg();
protected:
    int m_id;
private:
    QString m_msg;
};

typedef std::shared_ptr<Notification> NotificationPtr;

#endif // NOTIFICATION_H
