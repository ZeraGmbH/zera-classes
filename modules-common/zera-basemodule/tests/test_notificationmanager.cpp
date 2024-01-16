#include "test_notificationmanager.h"
#include "notification.h"
#include "notificationcontainer.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_notificationmanager)

void test_notificationmanager::init()
{
    TimerFactoryQtForTest::enableTest();
}

void test_notificationmanager::addOnePriorityNotif()
{
    NotificationContainer* notificationCtr = NotificationContainer::getInstance();
    NotificationPtr notif = std::make_shared<Notification>("Notification1");
    notificationCtr->addNotification(notif);

    QCOMPARE(notificationCtr->NotificationsListSize(), 1);
    notificationCtr->removeNotification(notif->getId());

    QCOMPARE(notificationCtr->NotificationsListSize(), 0);
}

void test_notificationmanager::addTwoPriorityNotifs()
{
    NotificationContainer* notificationCtr = NotificationContainer::getInstance();
    NotificationPtr notif = std::make_shared<Notification>("Notification1");
    NotificationPtr notif2 = std::make_shared<Notification>("Notification2");
    notificationCtr->addNotification(notif);
    notificationCtr->addNotification(notif2);

    QCOMPARE(notificationCtr->NotificationsListSize(), 2);

    notificationCtr->removeNotification(notif->getId());
    QCOMPARE(notificationCtr->NotificationsListSize(), 1);

    notificationCtr->removeNotification(notif2->getId());
    QCOMPARE(notificationCtr->NotificationsListSize(), 0);

}
