#ifndef TEST_NOTIFICATIONMANAGER_H
#define TEST_NOTIFICATIONMANAGER_H

#include <QObject>

class test_notificationmanager : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void addOnePriorityNotif();
    void addTwoPriorityNotifs();

};

#endif // TEST_NOTIFICATIONMANAGER_H
