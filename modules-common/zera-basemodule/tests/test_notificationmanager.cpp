#include "test_notificationmanager.h"
#include "notification.h"
#include "nonprioritynotification.h"
#include "notificationcontainer.h"
#include <timerfactoryqtfortest.h>
#include <timemachinefortest.h>
#include <QTest>

QTEST_MAIN(test_notificationmanager)

void test_notificationmanager::init()
{
    TimerFactoryQtForTest::enableTest();
}

void test_notificationmanager::addOneNonPriorityNotif()
{
    NotificationContainer* notificationCtr = NotificationContainer::getInstance();

    NotificationPtr notif = std::make_shared<NonPriorityNotification>("Notification1");
    notificationCtr->addNotification(notif);
    QCOMPARE(notificationCtr->NotificationsListSize(), 1);

    TimeMachineForTest::getInstance()->processTimers(500);
    QCOMPARE(notificationCtr->NotificationsListSize(), 0);
}

void test_notificationmanager::addOnePriorityNotif()
{
    NotificationContainer* notificationCtr = NotificationContainer::getInstance();
    NotificationPtr notif = std::make_shared<Notification>("Notification1");
    notificationCtr->addNotification(notif);

    QCOMPARE(notificationCtr->NotificationsListSize(), 1);
    notificationCtr->notifRemoved(notif->getId());

    QCOMPARE(notificationCtr->NotificationsListSize(), 0);
}

void test_notificationmanager::addOnePriorityOneNonPriorityNotif()
{
    NotificationContainer* notificationCtr = NotificationContainer::getInstance();
    NotificationPtr notif = std::make_shared<Notification>("Notification1");
    NotificationPtr notif2 = std::make_shared<NonPriorityNotification>("Notification2");

    notificationCtr->addNotification(notif);
    notificationCtr->addNotification(notif2);

    QCOMPARE(notificationCtr->NotificationsListSize(), 2);

    TimeMachineForTest::getInstance()->processTimers(500);
    QCOMPARE(notificationCtr->NotificationsListSize(), 1);

    notificationCtr->notifRemoved(notif->getId());
    QCOMPARE(notificationCtr->NotificationsListSize(), 0);
}

void test_notificationmanager::addTwoNonPriorityNotifs()
{
    NotificationContainer* notificationCtr = NotificationContainer::getInstance();
    NotificationPtr notif1 = std::make_shared<NonPriorityNotification>("Notification1");
    NotificationPtr notif2 = std::make_shared<NonPriorityNotification>("Notification2");

    notificationCtr->addNotification(notif1);
    notificationCtr->addNotification(notif2);
    QCOMPARE(notificationCtr->NotificationsListSize(), 2);

    TimeMachineForTest::getInstance()->processTimers(500);
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

    notificationCtr->notifRemoved(notif->getId());
    QCOMPARE(notificationCtr->NotificationsListSize(), 1);

    notificationCtr->notifRemoved(notif2->getId());
    QCOMPARE(notificationCtr->NotificationsListSize(), 0);

}
